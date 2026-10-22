#include "SkillComponent.h"
#include "SkillData.h"
#include "StatComponent.h"
#include "MonsterBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool USkillComponent::CanUse(const USkillData* Skill, float ReadyTime) const
{
	if (!Skill || !GetWorld()) return false;

	if (UStatComponent* Stat = GetOwner()->FindComponentByClass<UStatComponent>())
	{
		if (Stat->IsDead()) return false;
	}

	return GetWorld()->GetTimeSeconds() >= ReadyTime;
}

float USkillComponent::GetRatio(const USkillData* Skill, float ReadyTime) const
{
	if (!Skill || Skill->Cooldown <= 0.f || !GetWorld()) return 0.f;

	const float Remaining = ReadyTime - GetWorld()->GetTimeSeconds();
	return FMath::Clamp(Remaining / Skill->Cooldown, 0.f, 1.f);
}

void USkillComponent::PlayCameraShake(const USkillData* Skill)
{
	if (!Skill || !Skill->CameraShake) return;

	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
		{
			PC->ClientStartCameraShake(Skill->CameraShake);
		}
	}
}

// ───────── 회전베기 ─────────
void USkillComponent::UseSpin()
{
	if (!CanUse(SpinSkill, SpinReadyTime)) return;
	SpinReadyTime = GetWorld()->GetTimeSeconds() + SpinSkill->Cooldown;

	ACharacter* MyChar = Cast<ACharacter>(GetOwner());
	if (!MyChar) return;

	// 1) 모션 먼저
	if (SpinSkill->Montage)
	{
		MyChar->PlayAnimMontage(SpinSkill->Montage);
	}
	else
	{
		MeshBaseYaw = MyChar->GetMesh()->GetRelativeRotation().Yaw;
		SpinElapsed = 0.f;
		GetWorld()->GetTimerManager().SetTimer(
			SpinVisualTimer, this, &USkillComponent::TickSpinVisual, 0.01f, true);
	}

	// 2) 내려치는 타이밍에 판정
	if (SpinSkill->HitDelay > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			SpinHitTimer, this, &USkillComponent::PerformSpinHit, SpinSkill->HitDelay, false);
	}
	else
	{
		PerformSpinHit();
	}
}

void USkillComponent::PerformSpinHit()
{
	ACharacter* MyChar = Cast<ACharacter>(GetOwner());
	if (!MyChar || !SpinSkill) return;

	UStatComponent* MyStat = MyChar->FindComponentByClass<UStatComponent>();
	if (MyStat && MyStat->IsDead()) return;   // 시전 중 죽었으면 취소

	const FVector Center = MyChar->GetActorLocation();

	// 범위 안 몬스터 찾기
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> Ignore;
	Ignore.Add(MyChar);

	TArray<AActor*> Hits;
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(), Center, SpinSkill->Radius, ObjectTypes,
		AMonsterBase::StaticClass(), Ignore, Hits);

	const float Damage = (MyStat ? MyStat->AttackPower : 10.f) * SpinSkill->DamageMultiplier;

	// 데미지 + 넉백
	for (AActor* Hit : Hits)
	{
		if (UStatComponent* TargetStat = Hit->FindComponentByClass<UStatComponent>())
		{
			TargetStat->ApplyDamage(Damage, MyChar);
		}

		if (ACharacter* TargetChar = Cast<ACharacter>(Hit))
		{
			const FVector Dir = (Hit->GetActorLocation() - Center).GetSafeNormal2D();
			const float K = SpinSkill->KnockbackStrength;
			TargetChar->LaunchCharacter(Dir * K + FVector(0.f, 0.f, K * 0.35f), true, true);
		}
	}

	// 이펙트 + 카메라 흔들림 + 히트스톱
	if (SpinSkill->Effect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), SpinSkill->Effect, Center, FRotator::ZeroRotator, FVector(SpinSkill->EffectScale));
	}
	PlayCameraShake(SpinSkill);
	if (SpinSkill->Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SpinSkill->Sound, Center);
	}
	

	if (Hits.Num() > 0)
	{
		StartHitStop(SpinSkill->HitStopDuration);
	}
}


void USkillComponent::TickSpinVisual()
{
	ACharacter* MyChar = Cast<ACharacter>(GetOwner());
	if (!MyChar) return;

	SpinElapsed += 0.01f;
	const float Alpha = FMath::Clamp(SpinElapsed / SpinVisualDuration, 0.f, 1.f);

	FRotator Rot = MyChar->GetMesh()->GetRelativeRotation();
	Rot.Yaw = MeshBaseYaw + 360.f * Alpha;
	MyChar->GetMesh()->SetRelativeRotation(Rot);

	if (Alpha >= 1.f)
	{
		Rot.Yaw = MeshBaseYaw;
		MyChar->GetMesh()->SetRelativeRotation(Rot);
		GetWorld()->GetTimerManager().ClearTimer(SpinVisualTimer);
	}
}

// ───────── 히트스톱 ─────────
void USkillComponent::StartHitStop(float Duration)
{
	if (Duration <= 0.f) return;

	const float Dilation = 0.05f;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), Dilation);

	// 타이머도 느려지므로 실제 시간 기준으로 환산
	GetWorld()->GetTimerManager().SetTimer(
		HitStopTimer, this, &USkillComponent::EndHitStop, Duration * Dilation, false);
}

void USkillComponent::EndHitStop()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
}

// ───────── 대시 ─────────
void USkillComponent::UseDash()
{
	if (!CanUse(DashSkill, DashReadyTime)) return;

	ACharacter* MyChar = Cast<ACharacter>(GetOwner());
	if (!MyChar) return;

	DashReadyTime = GetWorld()->GetTimeSeconds() + DashSkill->Cooldown;

	FVector Dir = MyChar->GetLastMovementInputVector();
	if (Dir.IsNearlyZero())
	{
		Dir = MyChar->GetActorForwardVector();
	}
	Dir.Z = 0.f;
	Dir.Normalize();

	// 마찰을 잠깐 꺼서 쭉 미끄러지게
	UCharacterMovementComponent* Move = MyChar->GetCharacterMovement();
	if (!GetWorld()->GetTimerManager().IsTimerActive(DashSlideTimer))
	{
		SavedGroundFriction = Move->GroundFriction;
		SavedBrakingDecel = Move->BrakingDecelerationWalking;
	}
	Move->GroundFriction = 0.f;
	Move->BrakingDecelerationWalking = 0.f;

	MyChar->LaunchCharacter(Dir * DashSkill->DashStrength, true, false);

	GetWorld()->GetTimerManager().SetTimer(
		DashSlideTimer, this, &USkillComponent::EndDashSlide, DashSlideDuration, false);

	if (DashSkill->Effect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), DashSkill->Effect, MyChar->GetActorLocation(), FRotator::ZeroRotator, FVector(DashSkill->EffectScale));
	}
	PlayCameraShake(DashSkill);
	if (DashSkill->Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DashSkill->Sound, MyChar->GetActorLocation());
	}
}

void USkillComponent::EndDashSlide()
{
	ACharacter* MyChar = Cast<ACharacter>(GetOwner());
	if (!MyChar) return;

	UCharacterMovementComponent* Move = MyChar->GetCharacterMovement();
	Move->GroundFriction = SavedGroundFriction;
	Move->BrakingDecelerationWalking = SavedBrakingDecel;
	Move->Velocity *= 0.3f;   // 끝날 때 급정지 느낌
}