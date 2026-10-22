#include "BTTask_FindWanderLocation.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindWanderLocation::UBTTask_FindWanderLocation()
{
	NodeName = TEXT("Find Wander Location");

	WanderLocationKey.AddVectorFilter(
		this, GET_MEMBER_NAME_CHECKED(UBTTask_FindWanderLocation, WanderLocationKey));
}

EBTNodeResult::Type UBTTask_FindWanderLocation::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* Pawn = AICon->GetPawn();
	if (!Pawn) return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Pawn->GetWorld());
	if (!NavSys) return EBTNodeResult::Failed;

	// 현재 위치 주변에서 걸어갈 수 있는 랜덤 지점
	FNavLocation Result;
	if (!NavSys->GetRandomReachablePointInRadius(Pawn->GetActorLocation(), WanderRadius, Result))
	{
		return EBTNodeResult::Failed;
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(
		WanderLocationKey.SelectedKeyName, Result.Location);

	return EBTNodeResult::Succeeded;
}