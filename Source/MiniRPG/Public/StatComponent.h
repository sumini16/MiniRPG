// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnExpChangedSignature, int32, CurrentExp, int32, RequiredExp, int32, Level);
DECLARE_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDamagedSignature, AActor*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, Current, float, Max);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelUpSignature, int32);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )

class MINIRPG_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatComponent();
	
	
	UPROPERTY(BlueprintAssignable, Category = "Stat")
	FOnHealthChangedSignature OnHealthChanged;
	
	FOnDamagedSignature OnDamaged;
	FOnDeathSignature OnDeath;

	FOnLevelUpSignature OnLevelUp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Stat")
	int32 CurrentExp = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	int32 ExpReward = 20;

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void AddExp(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Stat")
	int32 GetRequiredExp() const;

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void ResetStat();
	
	UPROPERTY(BlueprintAssignable, Category = "Stat")
    FOnExpChangedSignature OnExpChanged;
	


	

	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

	void LevelUp();

	
public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Transient, Category = "Stat")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float AttackPower = 10.f;

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void ApplyDamage(float DamageAmount, AActor* Attacker = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Stat")
	bool IsDead() const;
	
};
