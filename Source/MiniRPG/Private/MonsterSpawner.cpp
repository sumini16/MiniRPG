#include "MonsterSpawner.h"
#include "MonsterBase.h"
#include "StatComponent.h"
#include "NavigationSystem.h"
#include "Components/BillboardComponent.h"

AMonsterSpawner::AMonsterSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

#if WITH_EDITORONLY_DATA
	UBillboardComponent* Billboard =
		CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(RootComponent);
#endif
}

void AMonsterSpawner::BeginPlay()
{
	if (!bUsePooling) return;   // 풀링 OFF면 미리 만들지 않음
	Super::BeginPlay();
	InitializePool();
	// 스폰 시작은 GameMode가 StartSpawning()으로 지시한다
}

void AMonsterSpawner::InitializePool()
{
	if (!MonsterClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (int32 i = 0; i < PoolSize; ++i)
	{
		AMonsterBase* Monster = World->SpawnActor<AMonsterBase>(
			MonsterClass, GetActorLocation(), FRotator::ZeroRotator, Params);

		if (!Monster) continue;

		// 몬스터 사망을 구독해 GameMode에 전달 (풀에 있는 동안 한 번만 구독)
		if (UStatComponent* Stat = Monster->FindComponentByClass<UStatComponent>())
		{
			Stat->OnDeath.AddUObject(this, &AMonsterSpawner::HandleMonsterDeath);
		}

		Monster->DeactivateMonster();
		MonsterPool.Add(Monster);
	}
}

void AMonsterSpawner::StartSpawning(int32 InTotalToSpawn, int32 InMaxAlive, float InInterval, UMonsterData* InMonsterData)
{
	TotalToSpawn = InTotalToSpawn;
	MaxAliveCount = InMaxAlive;
	SpawnInterval = InInterval;
	WaveMonsterData = InMonsterData;
	SpawnedCount = 0;

	GetWorldTimerManager().SetTimer(
		SpawnTimer, this, &AMonsterSpawner::TrySpawn, SpawnInterval, true, 0.5f);
}

void AMonsterSpawner::StopSpawning()
{
	GetWorldTimerManager().ClearTimer(SpawnTimer);
}

void AMonsterSpawner::HandleMonsterDeath()
{
	OnMonsterKilled.Broadcast();
}

AMonsterBase* AMonsterSpawner::GetInactiveMonster()
{
	for (AMonsterBase* Monster : MonsterPool)
	{
		if (IsValid(Monster) && !Monster->IsActiveMonster())
		{
			return Monster;
		}
	}
	return nullptr;
}

FVector AMonsterSpawner::GetRandomSpawnLocation() const
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys) return GetActorLocation();

	FNavLocation Result;
	if (NavSys->GetRandomReachablePointInRadius(GetActorLocation(), SpawnRadius, Result))
	{
		return Result.Location + FVector(0.f, 0.f, 90.f);
	}
	return GetActorLocation();
}

void AMonsterSpawner::TrySpawn()
{
	// 파괴된 몬스터(풀링 OFF) 정리
	MonsterPool.RemoveAll([](AMonsterBase* M) { return !IsValid(M); });

	if (SpawnedCount >= TotalToSpawn)
	{
		StopSpawning();
		return;
	}

	int32 AliveCount = 0;
	for (AMonsterBase* Monster : MonsterPool)
	{
		if (IsValid(Monster) && Monster->IsActiveMonster()) ++AliveCount;
	}
	if (AliveCount >= MaxAliveCount) return;

	const FVector SpawnLocation = GetRandomSpawnLocation();

	// ── 측정 시작 ──
	const double StartTime = FPlatformTime::Seconds();

	AMonsterBase* Monster = nullptr;

	if (bUsePooling)
	{
		Monster = GetInactiveMonster();
		if (!Monster) return;
	}
	else
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		Monster = GetWorld()->SpawnActor<AMonsterBase>(
			MonsterClass, SpawnLocation, FRotator::ZeroRotator, Params);
		if (!Monster) return;

		Monster->bDestroyOnDeactivate = true;

		if (UStatComponent* Stat = Monster->FindComponentByClass<UStatComponent>())
		{
			Stat->OnDeath.AddUObject(this, &AMonsterSpawner::HandleMonsterDeath);
		}
		MonsterPool.Add(Monster);
	}

	if (WaveMonsterData)
	{
		Monster->SetMonsterData(WaveMonsterData);
	}
	Monster->ActivateMonster(SpawnLocation);

	// ── 측정 끝 ──
	const double Ms = (FPlatformTime::Seconds() - StartTime) * 1000.0;
	TotalSpawnMs += Ms;
	++SpawnSamples;

	UE_LOG(LogTemp, Warning, TEXT("[SpawnCost] %s  this: %.3f ms  avg: %.3f ms  (n=%d)"),
		bUsePooling ? TEXT("POOL") : TEXT("NO-POOL"),
		Ms, TotalSpawnMs / SpawnSamples, SpawnSamples);

	++SpawnedCount;
}