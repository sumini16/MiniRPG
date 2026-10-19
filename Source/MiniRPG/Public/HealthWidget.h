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

protected:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UFUNCTION()
	void HandleHealthChanged(float Current, float Max);

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ExpBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LevelText;

	UFUNCTION()
	void HandleExpChanged(int32 CurrentExp, int32 RequiredExp, int32 Level);
};