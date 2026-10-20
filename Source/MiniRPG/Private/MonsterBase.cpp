// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterBase.h"
#include "StatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "HealthWidget.h"
#include "BrainComponent.h"
#include "QuestComponent.h"
#include "MonsterData.h"


void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	ApplyMonsterData();

	if (UStatComponent* Stat = FindComponentByClass<UStatComponent>())
	{
		Stat->OnDeath.AddUObject(this, &AMonsterBase::HandleDeath);
		Stat->OnDamaged.AddUObject(this, &AMonsterBase::HandleDamaged);

		if (UHealthWidget* HW = Cast<UHealthWidget>(HealthBarWidget->GetUserWidgetObject()))
		{
			HW->BindStatComponent(Stat);
		}
		
	}
}
AMonsterBase::AMonsterBase()
{
	if (UStatComponent* Stat = GetStatComponent())
	{
		Stat->MaxHealth = 30.f;
	}

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
	HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 110.f));
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidget->SetDrawSize(FVector2D(120.f, 15.f));
	
}

void AMonsterBase::HandleDeath()
{
	if (LastAttacker)
	{
		if (UStatComponent* KillerStat = LastAttacker->FindComponentByClass<UStatComponent>())
		{
			UStatComponent* MyStat = FindComponentByClass<UStatComponent>();
			KillerStat->AddExp(MyStat ? MyStat->ExpReward : 20);
		}

		if (UQuestComponent* Quest = LastAttacker->FindComponentByClass<UQuestComponent>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Quest comp: %s"), Quest ? TEXT("OK") : TEXT("NULL"));
			if (Quest) Quest->AddProgress(1);
		}
	}

	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
	
	GetWorldTimerManager().SetTimer(
		DeactivateTimer,
		[this]() { DeactivateMonster(); },
		3.f, false);

	
}

void AMonsterBase::TryAttack(AActor* Target)
{
	if (!bCanAttack) return;
	UStatComponent* MyStat = FindComponentByClass<UStatComponent>();
	if (MyStat && MyStat->IsDead()) return;

	// 타겟 쪽으로 몸을 돌린 뒤 공격 (트레이스가 앞쪽으로 나가므로)
	if (Target)
	{
		const FVector Dir = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		SetActorRotation(FRotator(0.f, Dir.Rotation().Yaw, 0.f));
	}

	Attack();

	bCanAttack = false;
	GetWorldTimerManager().SetTimer(
		AttackCooldownTimer,
		[this]() { bCanAttack = true; },
		AttackCooldown,
		false);
}

void AMonsterBase::HandleDamaged(AActor* Attacker)
{
	if (!Attacker || Attacker == this) return;
	LastAttacker = Attacker;
	
	AAIController* AICon = Cast<AAIController>(GetController());
	if (!AICon) return;

	if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
	{
		BB->SetValueAsObject(TEXT("TargetActor"), Attacker);
	}
}

void AMonsterBase::ActivateMonster(const FVector& SpawnLocation)
{
	bIsActive = true;

	// 1) 콜리전·이동을 먼저 켬
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 추가
	SetActorTickEnabled(true);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	// 2) 그 다음 위치 이동 — 순간이동으로 강제 배치
	SetActorLocation(SpawnLocation, false, nullptr, ETeleportType::TeleportPhysics);

	// 3) 비활성 상태에서 누적된 낙하 속도 제거
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->Velocity = FVector::ZeroVector;

	ApplyMonsterData();

	if (HealthBarWidget) HealthBarWidget->SetVisibility(true);

	LastAttacker = nullptr;
	bCanAttack = true;

	// AI 다시 시작
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
		{
			BB->ClearValue(TEXT("TargetActor"));
		}

		if (UBrainComponent* Brain = AICon->GetBrainComponent())
		{
			Brain->RestartLogic();
		}
	}
}

void AMonsterBase::DeactivateMonster()
{
	bIsActive = false;

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	if (HealthBarWidget) HealthBarWidget->SetVisibility(false);

	GetCharacterMovement()->DisableMovement();

	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* Brain = AICon->GetBrainComponent())
		{
			Brain->StopLogic(TEXT("Deactivated"));
		}
	}
}
	void AMonsterBase::ApplyMonsterData()
	{
		UStatComponent* Stat = FindComponentByClass<UStatComponent>();

		if (MonsterData)
		{
			if (Stat)
			{
				Stat->MaxHealth = MonsterData->MaxHealth;
				Stat->AttackPower = MonsterData->AttackPower;
				Stat->ExpReward = MonsterData->ExpReward;
			}

			AttackTriggerRange = MonsterData->AttackTriggerRange;
			AttackCooldown = MonsterData->AttackCooldown;
			AttackRange = MonsterData->AttackRange;
			AttackRadius = MonsterData->AttackRadius;

			SetActorScale3D(FVector(MonsterData->Scale));
		}

		if (Stat)
		{
			Stat->ResetStat();
		}
	}
