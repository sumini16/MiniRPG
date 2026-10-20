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
	
	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	float MoveSpeed = 600.f;

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
	
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	class USkeletalMesh* Mesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TSubclassOf<class UAnimInstance> AnimClass;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FVector MeshOffset = FVector(0.f, 0.f, -90.f);

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	float MeshYaw = -90.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	class UAnimMontage* AttackMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackHitDelay = 0.f;
};