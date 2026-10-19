#include "MiniRPGPlayerController.h"
#include "HealthWidget.h"
#include "CharacterBase.h"
#include "StatComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "MiniRPGGameMode.h"


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
	APawn* MyPawn = GetPawn();
	if (!MyPawn) return;

	UStatComponent* Stat = MyPawn->FindComponentByClass<UStatComponent>();
	if (!Stat) return;

	// 사망 알림 구독 (BeginPlay·OnPossess 두 번 호출돼도 한 번만)
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
		// 1) 조작 차단
		MyChar->DisableInput(this);
		MyChar->GetCharacterMovement()->DisableMovement();

		// 2) 래그돌로 쓰러지기
		MyChar->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MyChar->GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		MyChar->GetMesh()->SetSimulatePhysics(true);
	}

	// 3) 쓰러지는 모습을 보여준 뒤 UI 표시
	GetWorldTimerManager().SetTimer(
		GameOverTimer, this, &AMiniRPGPlayerController::ShowGameOver, GameOverDelay, false);
}

void AMiniRPGPlayerController::ShowGameOver()
{
	if (!GameOverWidgetClass || GameOverWidget) return;

	GameOverWidget = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
	if (!GameOverWidget) return;

	GameOverWidget->AddToViewport(10);   // HUD보다 위에 그리기

	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());
}