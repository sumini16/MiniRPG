#include "MiniRPGGameMode.h"
#include "MonsterSpawner.h"
#include "MonsterBase.h"
#include "StatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AMiniRPGGameMode::AMiniRPGGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	FWaveInfo Wave1;
	Wave1.MonsterCount = 5;
	Wave1.MaxAlive = 3;
	Wave1.SpawnInterval = 2.f;

	FWaveInfo Wave2;
	Wave2.MonsterCount = 8;
	Wave2.MaxAlive = 5;
	Wave2.SpawnInterval = 1.5f;

	Waves = { Wave1, Wave2 };
}

void AMiniRPGGameMode::BeginPlay()
{
	Super::BeginPlay();

	Spawner = Cast<AMonsterSpawner>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AMonsterSpawner::StaticClass()));

	if (!Spawner)
	{
		UE_LOG(LogTemp, Error, TEXT("MonsterSpawner not found in level"));
		return;
	}

	Spawner->OnMonsterKilled.AddUObject(this, &AMiniRPGGameMode::HandleMonsterKilled);

	GetWorldTimerManager().SetTimer(
		WaveTimer, this, &AMiniRPGGameMode::StartNextWave, FirstWaveDelay, false);
}

void AMiniRPGGameMode::StartNextWave()
{
	++CurrentWaveIndex;
	KilledInWave = 0;

	// 모든 웨이브를 넘겼으면 보스
	if (!Waves.IsValidIndex(CurrentWaveIndex))
	{
		SpawnBoss();
		return;
	}

	const FWaveInfo& Wave = Waves[CurrentWaveIndex];
	Spawner->StartSpawning(Wave.MonsterCount, Wave.MaxAlive, Wave.SpawnInterval, Wave.MonsterData);
	BroadcastProgress();
}

void AMiniRPGGameMode::HandleMonsterKilled()
{
	if (!Waves.IsValidIndex(CurrentWaveIndex)) return;

	++KilledInWave;
	BroadcastProgress();

	if (KilledInWave >= Waves[CurrentWaveIndex].MonsterCount)
	{
		Spawner->StopSpawning();
		GetWorldTimerManager().SetTimer(
			WaveTimer, this, &AMiniRPGGameMode::StartNextWave, TimeBetweenWaves, false);
	}
}

void AMiniRPGGameMode::BroadcastProgress()
{
	if (!Waves.IsValidIndex(CurrentWaveIndex)) return;

	OnWaveProgress.Broadcast(
		CurrentWaveIndex + 1, KilledInWave, Waves[CurrentWaveIndex].MonsterCount);
}

void AMiniRPGGameMode::SpawnBoss()
{
	if (!BossClass || !Spawner)
	{
		UE_LOG(LogTemp, Error, TEXT("BossClass not set"));
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const FVector Location = Spawner->GetActorLocation() + FVector(0.f, 0.f, 150.f);
	Boss = GetWorld()->SpawnActor<AMonsterBase>(BossClass, Location, FRotator::ZeroRotator, Params);
	if (!Boss) return;

	// 보스는 풀 밖의 개별 몬스터라 GameMode가 직접 사망을 구독
	if (UStatComponent* Stat = Boss->FindComponentByClass<UStatComponent>())
	{
		Stat->OnDeath.AddUObject(this, &AMiniRPGGameMode::HandleBossDeath);
	}

	Boss->ActivateMonster(Location);
	OnBossSpawned.Broadcast();
}

void AMiniRPGGameMode::HandleBossDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Boss defeated - Game Clear"));
	OnGameCleared.Broadcast();
}