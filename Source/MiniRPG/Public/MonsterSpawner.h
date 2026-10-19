#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnMonsterKilledSignature);

UCLASS()
class MINIRPG_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()

public:
	AMonsterSpawner();

	FOnMonsterKilledSignature OnMonsterKilled;

	void StartSpawning(int32 InTotalToSpawn, int32 InMaxAlive, float InInterval);
	void StopSpawning();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<class AMonsterBase> MonsterClass;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 PoolSize = 10;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float SpawnRadius = 600.f;

	UPROPERTY()
	TArray<class AMonsterBase*> MonsterPool;

	int32 MaxAliveCount = 4;
	float SpawnInterval = 3.f;
	int32 TotalToSpawn = 0;
	int32 SpawnedCount = 0;

	FTimerHandle SpawnTimer;

	void InitializePool();
	void TrySpawn();
	void HandleMonsterDeath();
	class AMonsterBase* GetInactiveMonster();
	FVector GetRandomSpawnLocation() const;
};