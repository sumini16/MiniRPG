#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MonsterData.generated.h"

UCLASS(BlueprintType)
class MINIRPG_API UMonsterData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	float MaxHealth = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	float AttackPower = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	int32 ExpReward = 20;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackTriggerRange = 180.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackCooldown = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackRange = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackRadius = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	float Scale = 1.f;
};