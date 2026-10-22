// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "MonsterBase.generated.h"

/**
 * 
 */
UCLASS()
class MINIRPG_API AMonsterBase : public ACharacterBase
{
	GENERATED_BODY()
public:
	AMonsterBase();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void TryAttack(AActor* Target);

	UFUNCTION(BlueprintPure, Category = "Combat")
	float GetAttackTriggerRange() const { return AttackTriggerRange; }
	
	void ActivateMonster(const FVector& SpawnLocation);
    void DeactivateMonster();
	void SetMonsterData(class UMonsterData* InData) { MonsterData = InData; }
    FORCEINLINE bool IsActiveMonster() const { return bIsActive; }
        
protected:
	virtual void BeginPlay() override;
	void HandleDeath();
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackCooldown = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackTriggerRange = 180.f;

	bool bCanAttack = true;
	FTimerHandle AttackCooldownTimer;

	void HandleDamaged(AActor* Attacker);

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* HealthBarWidget;

	UPROPERTY()
	AActor* LastAttacker;
	bool bIsActive = false;
    FTimerHandle DeactivateTimer;
	
	
	UPROPERTY(EditAnywhere, Category = "Monster")
	class UMonsterData* MonsterData;

	void ApplyMonsterData();
	
	FTimerHandle AttackHitTimer;
	void PerformAttackHit();
	
	FTimerHandle SpawnIntroTimer;
	
	FTimerHandle SlamLoopTimer;
	FTimerHandle SlamHitTimer;

	UPROPERTY()
	AActor* ActiveTelegraph;

	FVector SlamTarget;

	void StartSlam();
	void ExecuteSlam();
	void StopPattern();
};
