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
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

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
	GetWorldTimerManager().ClearTimer(AttackHitTimer);
	// 죽는 모션 재생
	
	StopPattern();
	
	if (MonsterData && MonsterData->DeathMontage)
	{
		PlayAnimMontage(MonsterData->DeathMontage);
	}
	
	if (MonsterData && MonsterData->DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, MonsterData->DeathSound, GetActorLocation());
	}
	

	// AI 즉시 정지 (시체가 움직이거나 공격하지 않게)
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->StopMovement();
		if (UBrainComponent* Brain = AICon->GetBrainComponent())
		{
			Brain->StopLogic(TEXT("Dead"));
		}
	}
	
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

	// 공격 모션 재생 → 휘두르는 타이밍에 판정
	if (MonsterData && MonsterData->AttackMontage)
	{
		PlayAnimMontage(MonsterData->AttackMontage);
		if (SwingSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, SwingSound, GetActorLocation());
		}
		
	}

	const float HitDelay = MonsterData ? MonsterData->AttackHitDelay : 0.f;
	if (HitDelay > 0.f)
	{
		GetWorldTimerManager().SetTimer(
			AttackHitTimer, this, &AMonsterBase::PerformAttackHit, HitDelay, false);
	}
	else
	{
		Attack();
	}

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
	
	UStatComponent* MyStat = FindComponentByClass<UStatComponent>();
	if (MonsterData && MonsterData->HitReactMontage && MyStat && !MyStat->IsDead())
	{
		PlayAnimMontage(MonsterData->HitReactMontage);
		GetWorldTimerManager().ClearTimer(AttackHitTimer);
	}
	
	AAIController* AICon = Cast<AAIController>(GetController());
	if (!AICon) return;

	if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
	{
		BB->SetValueAsObject(TEXT("TargetActor"), Attacker);
	}
}

void AMonsterBase::ActivateMonster(const FVector& SpawnLocation)
{
	// 풀에서 다시 꺼낼 때 쓰러진 자세 해제
	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		Anim->StopAllMontages(0.f);
	}
	
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
	
	// 등장 모션이 있으면 재생하는 동안 AI 정지 → 끝나면 추격 시작
	if (MonsterData && MonsterData->SpawnMontage)
	{
		const float Length = PlayAnimMontage(MonsterData->SpawnMontage);
		
		if (MonsterData->SpawnSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, MonsterData->SpawnSound, GetActorLocation());
		}

		if (AAIController* AICon = Cast<AAIController>(GetController()))
		{
			if (UBrainComponent* Brain = AICon->GetBrainComponent())
			{
				Brain->StopLogic(TEXT("SpawnIntro"));
			}
		}

		GetWorldTimerManager().SetTimer(
			SpawnIntroTimer,
			FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				if (AAIController* AICon = Cast<AAIController>(GetController()))
				{
					if (UBrainComponent* Brain = AICon->GetBrainComponent())
					{
						Brain->RestartLogic();
					}
				}
			}),
			FMath::Max(Length, 0.1f), false);
	}
	
	// 보스 장판 패턴 시작
	if (MonsterData && MonsterData->bUseSlamPattern)
	{
		GetWorldTimerManager().SetTimer(
			SlamLoopTimer, this, &AMonsterBase::StartSlam,
			MonsterData->SlamInterval, true);
	}
}

void AMonsterBase::DeactivateMonster()
{
	bIsActive = false;
	StopPattern();
	
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
	// 카메라가 몬스터 몸에 걸려 줌인되지 않게
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	
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
			GetCharacterMovement()->MaxWalkSpeed = MonsterData->MoveSpeed;
			SwingSound = MonsterData->SwingSound;
			ImpactSound = MonsterData->ImpactSound;
			
			

			SetActorScale3D(FVector(MonsterData->Scale));
			if (MonsterData->Mesh)
			{
				GetMesh()->SetSkeletalMeshAsset(MonsterData->Mesh);

				if (MonsterData->AnimClass)
				{
					GetMesh()->SetAnimInstanceClass(MonsterData->AnimClass);
				}

				GetMesh()->SetRelativeLocationAndRotation(
					MonsterData->MeshOffset, FRotator(0.f, MonsterData->MeshYaw, 0.f));
			}
		}

		if (Stat)
		{
			Stat->ResetStat();
		}
	}

void AMonsterBase::PerformAttackHit()
{
	UStatComponent* MyStat = FindComponentByClass<UStatComponent>();
	if (MyStat && MyStat->IsDead()) return;   // 휘두르는 도중 죽었으면 판정 없음

	Attack();
}

void AMonsterBase::StartSlam()
{
	UStatComponent* MyStat = FindComponentByClass<UStatComponent>();
	if (!MonsterData || (MyStat && MyStat->IsDead())) return;

	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!Player) return;

	if (UStatComponent* PlayerStat = Player->FindComponentByClass<UStatComponent>())
	{
		if (PlayerStat->IsDead()) return;
	}

	// 1) 플레이어 발밑 바닥 위치 구하기
	SlamTarget = Player->GetActorLocation();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Player);
	Params.AddIgnoredActor(this);

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(
		Hit, SlamTarget, SlamTarget - FVector(0.f, 0.f, 500.f), ECC_Visibility, Params))
	{
		SlamTarget = Hit.ImpactPoint;
	}

	// 2) 경고 원 표시
	if (MonsterData->TelegraphClass)
	{
		ActiveTelegraph = GetWorld()->SpawnActor<AActor>(
			MonsterData->TelegraphClass, SlamTarget + FVector(0.f, 0.f, 2.f), FRotator::ZeroRotator);

		if (ActiveTelegraph)
		{
			const float S = MonsterData->SlamRadius / 50.f;   // 기본 원통 반지름 50 기준
			ActiveTelegraph->SetActorScale3D(FVector(S, S, 0.02f));
		}
	}

	// 3) 그쪽을 보고 내려찍기 모션
	const FVector Dir = (SlamTarget - GetActorLocation()).GetSafeNormal2D();
	SetActorRotation(Dir.Rotation());

	if (MonsterData->SlamMontage)
	{
		PlayAnimMontage(MonsterData->SlamMontage);
	}

	// 4) 경고 시간 뒤 판정
	GetWorldTimerManager().SetTimer(
		SlamHitTimer, this, &AMonsterBase::ExecuteSlam, MonsterData->SlamWarningTime, false);
}

void AMonsterBase::ExecuteSlam()
{
	if (ActiveTelegraph)
	{
		ActiveTelegraph->Destroy();
		ActiveTelegraph = nullptr;
	}

	UStatComponent* MyStat = FindComponentByClass<UStatComponent>();
	if (!MonsterData || (MyStat && MyStat->IsDead())) return;

	// 원 안에 있는 대상에게 피해 (몬스터 제외)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> Ignore;
	Ignore.Add(this);

	TArray<AActor*> Hits;
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(), SlamTarget, MonsterData->SlamRadius, ObjectTypes, nullptr, Ignore, Hits);

	const float Damage = (MyStat ? MyStat->AttackPower : 10.f) * MonsterData->SlamDamageMultiplier;

	for (AActor* Target : Hits)
	{
		if (Target->IsA<AMonsterBase>()) continue;

		if (UStatComponent* TargetStat = Target->FindComponentByClass<UStatComponent>())
		{
			TargetStat->ApplyDamage(Damage, this);
		}
	}

	if (MonsterData->SlamEffect)
	{
		const FVector Scale(MonsterData->SlamEffectScale);

		// 가운데 큰 폭발
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), MonsterData->SlamEffect, SlamTarget, FRotator::ZeroRotator, Scale);

		// 원 가장자리를 따라 작은 폭발 → 범위 전체를 덮음
		const int32 Count = 6;
		for (int32 i = 0; i < Count; ++i)
		{
			const float Angle = 2.f * PI * i / Count;
			const FVector Offset(FMath::Cos(Angle), FMath::Sin(Angle), 0.f);

			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(), MonsterData->SlamEffect,
				SlamTarget + Offset * MonsterData->SlamRadius * 0.65f,
				FRotator::ZeroRotator, Scale * 0.6f);
		}
	}

	if (MonsterData->SlamCameraShake)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			PC->ClientStartCameraShake(MonsterData->SlamCameraShake);
		}
	}
}

void AMonsterBase::StopPattern()
{
	GetWorldTimerManager().ClearTimer(SlamLoopTimer);
	GetWorldTimerManager().ClearTimer(SlamHitTimer);

	if (ActiveTelegraph)
	{
		ActiveTelegraph->Destroy();
		ActiveTelegraph = nullptr;
	}
}