// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CharacterBase.generated.h"


UCLASS()
class MINIRPG_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();
	FORCEINLINE class UStatComponent* GetStatComponent() const { return StatComponent; }
	FORCEINLINE class UQuestComponent* GetQuestComponent() const { return QuestComponent; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	class UStatComponent* StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;

	//매핑표. "W키는 IA_Move, 마우스는 IA_Look" 이런 연결 관계
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;
	//개별 행동 하나. "이동"이라는 행동, "시점"이라는 행동, "점프"라는 행동
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* LookAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* JumpAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* AttackAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")   
	class UInputAction* SkillQAction;                                     
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* DashAction;
	
	
	void UseSkillQ();
	void UseDash();
	void OnAttackInput();
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Attack();
	//공격
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackRange = 150.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackRadius = 50.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TArray<class UAnimMontage*> PlayerAttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float ComboResetTime = 1.2f;

	int32 ComboIndex = 0;
	FTimerHandle ComboResetTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float PlayerHitDelay = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float PlayerAttackInterval = 0.6f;

	bool bPlayerAttackLocked = false;
	FTimerHandle PlayerHitTimer;
	FTimerHandle PlayerAttackLockTimer;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bDrawAttackDebug = false;
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	class UQuestComponent* QuestComponent;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
