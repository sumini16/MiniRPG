#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"

UCLASS()
class MINIRPG_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()

public:
	AMonsterSpawner();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<class AMonsterBase> MonsterClass;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 PoolSize = 10;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 MaxAliveCount = 4;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float SpawnInterval = 3.f;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float SpawnRadius = 600.f;

	UPROPERTY()
	TArray<class AMonsterBase*> MonsterPool;

	FTimerHandle SpawnTimer;

	void InitializePool();
	void TrySpawn();
	class AMonsterBase* GetInactiveMonster();
	FVector GetRandomSpawnLocation() const;
};