#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MiniRPGPlayerController.generated.h"

UCLASS()
class MINIRPG_API AMiniRPGPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UHealthWidget> HealthWidgetClass;

	UPROPERTY()
	class UHealthWidget* HealthWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> GameOverWidgetClass;

	UPROPERTY()
	class UUserWidget* GameOverWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float GameOverDelay = 2.f;

	FTimerHandle GameOverTimer;

	void SetupHealthWidget();
	void HandlePlayerDeath();
	void ShowGameOver();
};