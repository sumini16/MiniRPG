#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestComponent.generated.h"

UENUM(BlueprintType)
enum class EQuestState : uint8
{
	NotAccepted,
	InProgress,
	ReadyToComplete,
	Completed
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnQuestUpdated, EQuestState, State, int32, Current, int32, Goal);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MINIRPG_API UQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UQuestComponent();

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestUpdated OnQuestUpdated;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FString QuestTitle = TEXT("몬스터 처치");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	int32 GoalCount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	int32 RewardExp = 100;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void AcceptQuest();

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void AddProgress(int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void CompleteQuest();

	UFUNCTION(BlueprintPure, Category = "Quest")
	EQuestState GetState() const { return State; }

	UFUNCTION(BlueprintPure, Category = "Quest")
	int32 GetCurrentCount() const { return CurrentCount; }

protected:
	UPROPERTY(VisibleAnywhere, Transient, Category = "Quest")
	EQuestState State = EQuestState::NotAccepted;

	UPROPERTY(VisibleAnywhere, Transient, Category = "Quest")
	int32 CurrentCount = 0;

	void Notify();
};