#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_FindWanderLocation.generated.h"

UCLASS()
class MINIRPG_API UBTTask_FindWanderLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindWanderLocation();

	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector WanderLocationKey;

	UPROPERTY(EditAnywhere, Category = "Wander")
	float WanderRadius = 800.f;
};