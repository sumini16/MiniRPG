#include "GameOverWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton)
	{
		RestartButton->OnClicked.AddUniqueDynamic(this, &UGameOverWidget::HandleRestartClicked);
	}
}

void UGameOverWidget::HandleRestartClicked()
{
	// 입력 모드를 게임용으로 되돌린 뒤 레벨 재시작
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}

	const FString LevelName = UGameplayStatics::GetCurrentLevelName(this, true);
	UGameplayStatics::OpenLevel(this, FName(*LevelName));
}