#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkillData.generated.h"

UCLASS(BlueprintType)
class MINIRPG_API USkillData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float Cooldown = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float DamageMultiplier = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float Radius = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float DashStrength = 2000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	class UAnimMontage* Montage = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float HitDelay = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	class UParticleSystem* Effect = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float EffectScale = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Feel")
	TSubclassOf<class UCameraShakeBase> CameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Feel")
	float KnockbackStrength = 900.f;

	UPROPERTY(EditDefaultsOnly, Category = "Feel")
	float HitStopDuration = 0.08f;
};