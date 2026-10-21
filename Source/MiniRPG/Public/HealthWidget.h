#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthWidget.generated.h"

UCLASS()
class MINIRPG_API UHealthWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindStatComponent(class UStatComponent* Stat);
	void BindWaveProgress(class AMiniRPGGameMode* GameMode);
	void BindSkillComponent(class USkillComponent* InSkill);

protected:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidgetOptional))
	class UProgressBar* ExpBar;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* LevelText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* WaveText;

	UFUNCTION()
	void HandleHealthChanged(float Current, float Max);

	UFUNCTION()
	void HandleExpChanged(int32 CurrentExp, int32 RequiredExp, int32 Level);

	UFUNCTION()
	void HandleWaveProgress(int32 WaveNumber, int32 Killed, int32 Total);

	void HandleBossSpawned();
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY()
	class USkillComponent* SkillComponent;

	UPROPERTY(meta = (BindWidgetOptional))
	class UProgressBar* SpinCooldownBar;

	UPROPERTY(meta = (BindWidgetOptional))
	class UProgressBar* DashCooldownBar;
	
	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* AnnounceText;

	FTimerHandle AnnounceTimer;
	void HandleAnnounce(const FText& Text);
};