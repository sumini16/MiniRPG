#include "HealthWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "StatComponent.h"
#include "MiniRPGGameMode.h"

void UHealthWidget::BindStatComponent(UStatComponent* Stat)
{
	if (!Stat) return;

	Stat->OnHealthChanged.AddUniqueDynamic(this, &UHealthWidget::HandleHealthChanged);
	Stat->OnExpChanged.AddUniqueDynamic(this, &UHealthWidget::HandleExpChanged);

	HandleHealthChanged(Stat->CurrentHealth, Stat->MaxHealth);
	HandleExpChanged(Stat->CurrentExp, Stat->GetRequiredExp(), Stat->Level);
}

void UHealthWidget::BindWaveProgress(AMiniRPGGameMode* GameMode)
{
	if (!GameMode) return;

	GameMode->OnWaveProgress.AddUniqueDynamic(this, &UHealthWidget::HandleWaveProgress);

	if (!GameMode->OnBossSpawned.IsBoundToObject(this))
	{
		GameMode->OnBossSpawned.AddUObject(this, &UHealthWidget::HandleBossSpawned);
	}
}

void UHealthWidget::HandleHealthChanged(float Current, float Max)
{
	if (!HealthBar || Max <= 0.f) return;

	HealthBar->SetPercent(Current / Max);
}

void UHealthWidget::HandleExpChanged(int32 CurrentExp, int32 RequiredExp, int32 Level)
{
	if (ExpBar && RequiredExp > 0)
	{
		ExpBar->SetPercent((float)CurrentExp / (float)RequiredExp);
	}

	if (LevelText)
	{
		LevelText->SetText(FText::FromString(FString::Printf(TEXT("Lv. %d"), Level)));
	}
}

void UHealthWidget::HandleWaveProgress(int32 WaveNumber, int32 Killed, int32 Total)
{
	if (!WaveText) return;

	WaveText->SetText(FText::FromString(
		FString::Printf(TEXT("WAVE %d   %d / %d"), WaveNumber, Killed, Total)));
}

void UHealthWidget::HandleBossSpawned()
{
	if (WaveText)
	{
		WaveText->SetText(FText::FromString(TEXT("BOSS")));
	}
}