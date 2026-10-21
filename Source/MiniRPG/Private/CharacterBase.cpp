// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "MonsterBase.h"
#include "StatComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetSystemLibrary.h"
#include "QuestComponent.h"
#include "SkillComponent.h"


ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	

	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	// 컨트롤러(마우스) 회전이 캐릭터 몸통을 직접 돌리지 않게 함
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	// 대신 캐릭터는 "이동하는 방향"을 바라보게 함
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm);
	FollowCamera->bUsePawnControlRotation = false;

	QuestComponent = CreateDefaultSubobject<UQuestComponent>(TEXT("QuestComponent"));
}


// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) //키보드/마우스 입력을 어떤 함수에 연결할지 정하는 곳
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACharacterBase::Move);
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACharacterBase::Look);

		EIC->BindAction(AttackAction, ETriggerEvent::Started, this, &ACharacterBase::OnAttackInput);
		
		if (SkillQAction)
		{
			EIC->BindAction(SkillQAction, ETriggerEvent::Started, this, &ACharacterBase::UseSkillQ);
		}
		if (DashAction)
		{
			EIC->BindAction(DashAction, ETriggerEvent::Started, this, &ACharacterBase::UseDash);
		}
	}

}

void ACharacterBase::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller == nullptr) return;

	const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ACharacterBase::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller == nullptr) return;

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ACharacterBase::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack!"));
	const FVector Start = GetActorLocation();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	TArray<FHitResult> HitResults;

	const bool bHit = UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		Start,
		End,
		AttackRadius,
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false,
		ActorsToIgnore,
		bDrawAttackDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResults,
		true
	);
	//공격 판정
	//UE_LOG(LogTemp, Warning, TEXT("bHit: %d, Count: %d"), bHit, HitResults.Num());
	if (!bHit) return;

	TSet<AActor*> DamagedActors;
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || DamagedActors.Contains(HitActor)) continue;

		UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *HitActor->GetName());

		ACharacterBase* Target = Cast<ACharacterBase>(HitActor);
		if (!Target)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cast FAILED"));
			continue;
		}

		
		// 같은 편(몬스터→몬스터) 공격 방지
		if (IsA<AMonsterBase>() && Target->IsA<AMonsterBase>())
		{
			continue;
		}
		

		UStatComponent* TargetStat = HitActor->FindComponentByClass<UStatComponent>();
		if (!TargetStat)
		{
			UE_LOG(LogTemp, Warning, TEXT("Stat NULL"));
			continue;
		}

		const float Damage = StatComponent ? StatComponent->AttackPower : 10.f;
		TargetStat->ApplyDamage(Damage, this);

		DamagedActors.Add(HitActor);
	}
}

void ACharacterBase::UseSkillQ()
{
	if (USkillComponent* Skill = FindComponentByClass<USkillComponent>())
	{
		Skill->UseSpin();
	}
}

void ACharacterBase::UseDash()
{
	if (USkillComponent* Skill = FindComponentByClass<USkillComponent>())
	{
		Skill->UseDash();
	}
}

void ACharacterBase::OnAttackInput()
{
	if (bPlayerAttackLocked) return;
	if (StatComponent && StatComponent->IsDead()) return;

	bPlayerAttackLocked = true;

	// 콤보: A → B → C 순서로 재생
	if (PlayerAttackMontages.Num() > 0)
	{
		if (UAnimMontage* Montage = PlayerAttackMontages[ComboIndex])
		{
			PlayAnimMontage(Montage);
		}
		ComboIndex = (ComboIndex + 1) % PlayerAttackMontages.Num();

		// 일정 시간 공격이 없으면 콤보 처음으로
		GetWorldTimerManager().SetTimer(
			ComboResetTimer, [this]() { ComboIndex = 0; }, ComboResetTime, false);
	}

	// 휘두르는 타이밍에 판정
	if (PlayerHitDelay > 0.f)
	{
		GetWorldTimerManager().SetTimer(
			PlayerHitTimer, this, &ACharacterBase::Attack, PlayerHitDelay, false);
	}
	else
	{
		Attack();
	}

	// 연타 방지: 일정 시간 후 다시 공격 가능
	GetWorldTimerManager().SetTimer(
		PlayerAttackLockTimer, [this]() { bPlayerAttackLocked = false; },
		PlayerAttackInterval, false);
}