#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MINIRPG_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USkillComponent();

	void UseSpin();
	void UseDash();

	float GetSpinCooldownRatio() const { return GetRatio(SpinSkill, SpinReadyTime); }
	float GetDashCooldownRatio() const { return GetRatio(DashSkill, DashReadyTime); }

protected:
	UPROPERTY(EditAnywhere, Category = "Skill")
	class USkillData* SpinSkill;

	UPROPERTY(EditAnywhere, Category = "Skill")
	class USkillData* DashSkill;

	UPROPERTY(EditAnywhere, Category = "Skill")
	float SpinVisualDuration = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Skill")
	float DashSlideDuration = 0.2f;

	float SpinReadyTime = 0.f;
	float DashReadyTime = 0.f;

	// 회전 연출
	FTimerHandle SpinVisualTimer;
	float SpinElapsed = 0.f;
	float MeshBaseYaw = 0.f;

	// 히트스톱
	FTimerHandle HitStopTimer;

	// 대시 미끄러짐
	FTimerHandle DashSlideTimer;
	float SavedGroundFriction = 8.f;
	float SavedBrakingDecel = 2048.f;

	bool CanUse(const class USkillData* Skill, float ReadyTime) const;
	float GetRatio(const class USkillData* Skill, float ReadyTime) const;

	void PlayCameraShake(const class USkillData* Skill);
	void StartHitStop(float Duration);
	void EndHitStop();
	void TickSpinVisual();
	void EndDashSlide();
	FTimerHandle SpinHitTimer;    
	void PerformSpinHit();         
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float HitDelay = 0.f;
};