#include "MiniRPGPlayerController.h"
#include "HealthWidget.h"
#include "CharacterBase.h"
#include "StatComponent.h"

void AMiniRPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HealthWidgetClass && !HealthWidget)
	{
		HealthWidget = CreateWidget<UHealthWidget>(this, HealthWidgetClass);
		if (HealthWidget) HealthWidget->AddToViewport();
	}

	SetupHealthWidget();
}

void AMiniRPGPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	SetupHealthWidget();
}

void AMiniRPGPlayerController::SetupHealthWidget()
{
	if (!HealthWidget) return;

	APawn* MyPawn = GetPawn();
	if (!MyPawn) return;

	if (UStatComponent* Stat = MyPawn->FindComponentByClass<UStatComponent>())
	{
		HealthWidget->BindStatComponent(Stat);
	}
}