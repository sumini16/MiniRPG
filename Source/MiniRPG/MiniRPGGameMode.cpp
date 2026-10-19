#include "MiniRPGGameMode.h"
#include "MonsterSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AMiniRPGGameMode::AMiniRPGGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// 기본 웨이브 구성 (BP 디테일 패널에서 조정 가능)
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

	// 스포너의 풀 생성(BeginPlay)이 끝난 뒤에 시작하도록 지연
	GetWorldTimerManager().SetTimer(
		WaveTimer, this, &AMiniRPGGameMode::StartNextWave, FirstWaveDelay, false);
}

void AMiniRPGGameMode::StartNextWave()
{
	++CurrentWaveIndex;
	KilledInWave = 0;

	if (!Waves.IsValidIndex(CurrentWaveIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("All waves cleared"));
		OnAllWavesCleared.Broadcast();
		return;
	}

	const FWaveInfo& Wave = Waves[CurrentWaveIndex];
	Spawner->StartSpawning(Wave.MonsterCount, Wave.MaxAlive, Wave.SpawnInterval);
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