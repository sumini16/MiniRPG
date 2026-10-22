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
	
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
    class UAnimMontage* DeathMontage = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	class UAnimMontage* SpawnMontage = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundBase* SwingSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundBase* ImpactSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundBase* DeathSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundBase* SpawnSound = nullptr;
	
	
	// ───── AI ─────
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float AggroFallbackTime = 15.f;   // 이 시간 동안 타겟이 없으면 플레이어 추격 (0 = 즉시)

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float AlertRadius = 1000.f;       // 발견·피격 시 이 반경의 동료도 추격
	
	
	
	// ───── Pattern (보스 장판) ─────
	UPROPERTY(EditDefaultsOnly, Category = "Pattern")
	bool bUseSlamPattern = false;

	UPROPERTY(EditDefaultsOnly, Category = "Pattern")
	float SlamInterval = 8.f;

	UPROPERTY(EditDefaultsOnly, Category = "Pattern")
	float SlamWarningTime = 1.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Pattern")
	float SlamRadius = 350.f;

	UPROPERTY(EditDefaultsOnly, Category = "Pattern")
	float SlamDamageMultiplier = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Pattern")
	class UAnimMontage* SlamMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Pattern")
	TSubclassOf<AActor> TelegraphClass;

	UPROPERTY(EditDefaultsOnly, Category = "Pattern")
	class UParticleSystem* SlamEffect = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Pattern")
	float SlamEffectScale = 3.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Pattern")
	TSubclassOf<class UCameraShakeBase> SlamCameraShake;
	
	
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	class UAnimMontage* HitReactMontage = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	class UAnimMontage* AttackMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackHitDelay = 0.f;
};