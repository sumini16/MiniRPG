#include "HealthWidget.h"
#include "Components/ProgressBar.h"
#include "StatComponent.h"
#include "Components/TextBlock.h"
void UHealthWidget::BindStatComponent(UStatComponent* Stat)
{
	if (!Stat) return;

	Stat->OnHealthChanged.AddDynamic(this, &UHealthWidget::HandleHealthChanged);
	Stat->OnExpChanged.AddDynamic(this, &UHealthWidget::HandleExpChanged);

	// 현재 상태로 즉시 갱신 (바인딩 시점에 이미 값이 있을 수 있으므로)
	HandleHealthChanged(Stat->CurrentHealth, Stat->MaxHealth);
	HandleExpChanged(Stat->CurrentExp, Stat->GetRequiredExp(), Stat->Level);
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