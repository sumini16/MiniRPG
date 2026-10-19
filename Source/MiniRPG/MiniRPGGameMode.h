#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MiniRPGGameMode.generated.h"

USTRUCT(BlueprintType)
struct FWaveInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Wave")
	int32 MonsterCount = 5;

	UPROPERTY(EditAnywhere, Category = "Wave")
	int32 MaxAlive = 3;

	UPROPERTY(EditAnywhere, Category = "Wave")
	float SpawnInterval = 2.f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWaveProgressSignature, int32, WaveNumber, int32, Killed, int32, Total);
DECLARE_MULTICAST_DELEGATE(FOnAllWavesClearedSignature);

UCLASS(minimalapi)
class AMiniRPGGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMiniRPGGameMode();

	UPROPERTY(BlueprintAssignable, Category = "Wave")
	FOnWaveProgressSignature OnWaveProgress;

	FOnAllWavesClearedSignature OnAllWavesCleared;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Wave")
	TArray<FWaveInfo> Waves;

	UPROPERTY(EditDefaultsOnly, Category = "Wave")
	float FirstWaveDelay = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Wave")
	float TimeBetweenWaves = 3.f;

	UPROPERTY()
	class AMonsterSpawner* Spawner;

	int32 CurrentWaveIndex = -1;
	int32 KilledInWave = 0;
	FTimerHandle WaveTimer;

	void StartNextWave();
	void HandleMonsterKilled();
	void BroadcastProgress();
};