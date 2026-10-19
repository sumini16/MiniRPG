#include "QuestComponent.h"
#include "StatComponent.h"

UQuestComponent::UQuestComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UQuestComponent::AcceptQuest()
{
	if (State != EQuestState::NotAccepted) return;

	State = EQuestState::InProgress;
	CurrentCount = 0;
	Notify();
}

void UQuestComponent::AddProgress(int32 Amount)
{
	if (State != EQuestState::InProgress) return;

	CurrentCount = FMath::Min(CurrentCount + Amount, GoalCount);

	if (CurrentCount >= GoalCount)
	{
		State = EQuestState::ReadyToComplete;
	}

	Notify();
}

void UQuestComponent::CompleteQuest()
{
	if (State != EQuestState::ReadyToComplete) return;

	State = EQuestState::Completed;

	if (UStatComponent* Stat = GetOwner()->FindComponentByClass<UStatComponent>())
	{
		Stat->AddExp(RewardExp);
	}

	Notify();

	UE_LOG(LogTemp, Warning, TEXT("Quest Completed! +%d Exp"), RewardExp);
}

void UQuestComponent::Notify()
{
	OnQuestUpdated.Broadcast(State, CurrentCount, GoalCount);
}