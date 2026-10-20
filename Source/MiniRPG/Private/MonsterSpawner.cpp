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
		if (Monster && !Monster->IsActiveMonster())
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
	// 이번 웨이브 할당량을 다 채웠으면 멈춤
	if (SpawnedCount >= TotalToSpawn)
	{
		StopSpawning();
		return;
	}

	int32 AliveCount = 0;
	for (AMonsterBase* Monster : MonsterPool)
	{
		if (Monster && Monster->IsActiveMonster()) ++AliveCount;
	}
	if (AliveCount >= MaxAliveCount) return;

	AMonsterBase* Monster = GetInactiveMonster();
	if (!Monster) return;
	
	// 이번 웨이브의 몬스터 데이터를 입혀서 내보냄
	if (WaveMonsterData)
	{
		Monster->SetMonsterData(WaveMonsterData);
	}

	Monster->ActivateMonster(GetRandomSpawnLocation());
	++SpawnedCount;
}