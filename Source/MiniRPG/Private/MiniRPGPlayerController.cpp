#include "MiniRPGPlayerController.h"
#include "HealthWidget.h"
#include "GameOverWidget.h"
#include "CharacterBase.h"
#include "StatComponent.h"
#include "MiniRPGGameMode.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

void AMiniRPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HealthWidgetClass && !HealthWidget)
	{
		HealthWidget = CreateWidget<UHealthWidget>(this, HealthWidgetClass);
		if (HealthWidget) HealthWidget->AddToViewport();
	}

	SetupHealthWidget();

	if (AMiniRPGGameMode* GM = GetWorld()->GetAuthGameMode<AMiniRPGGameMode>())
	{
		GM->OnGameCleared.AddUObject(this, &AMiniRPGPlayerController::HandleGameCleared);
	}
}

void AMiniRPGPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	SetupHealthWidget();
}

void AMiniRPGPlayerController::SetupHealthWidget()
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn) return;

	UStatComponent* Stat = MyPawn->FindComponentByClass<UStatComponent>();
	if (!Stat) return;

	if (!Stat->OnDeath.IsBoundToObject(this))
	{
		Stat->OnDeath.AddUObject(this, &AMiniRPGPlayerController::HandlePlayerDeath);
	}

	if (HealthWidget)
	{
		HealthWidget->BindStatComponent(Stat);

		if (AMiniRPGGameMode* GM = GetWorld()->GetAuthGameMode<AMiniRPGGameMode>())
		{
			HealthWidget->BindWaveProgress(GM);
		}
	}
}

void AMiniRPGPlayerController::HandlePlayerDeath()
{
	if (ACharacter* MyChar = GetCharacter())
	{
		MyChar->DisableInput(this);
		MyChar->GetCharacterMovement()->DisableMovement();

		MyChar->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MyChar->GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		MyChar->GetMesh()->SetSimulatePhysics(true);
	}

	GetWorldTimerManager().SetTimer(
		EndScreenTimer,
		FTimerDelegate::CreateUObject(this, &AMiniRPGPlayerController::ShowEndScreen, false),
		EndScreenDelay, false);
}

void AMiniRPGPlayerController::HandleGameCleared()
{
	if (APawn* MyPawn = GetPawn())
	{
		MyPawn->DisableInput(this);
	}

	GetWorldTimerManager().SetTimer(
		EndScreenTimer,
		FTimerDelegate::CreateUObject(this, &AMiniRPGPlayerController::ShowEndScreen, true),
		EndScreenDelay, false);
}

void AMiniRPGPlayerController::ShowEndScreen(bool bCleared)
{
	if (!GameOverWidgetClass || GameOverWidget) return;

	GameOverWidget = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
	if (!GameOverWidget) return;

	if (UGameOverWidget* EndWidget = Cast<UGameOverWidget>(GameOverWidget))
	{
		EndWidget->SetTitle(FText::FromString(bCleared ? TEXT("CLEAR") : TEXT("YOU DIED")));
	}

	GameOverWidget->AddToViewport(10);

	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());
}