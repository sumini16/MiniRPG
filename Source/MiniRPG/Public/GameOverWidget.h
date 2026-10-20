#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

UCLASS()
class MINIRPG_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetTitle(const FText& InTitle);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UButton* RestartButton;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* TitleText;

	UFUNCTION()
	void HandleRestartClicked();
};