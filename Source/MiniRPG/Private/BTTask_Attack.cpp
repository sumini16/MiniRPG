#include "BTTask_Attack.h"
#include "MonsterBase.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack");

	TargetKey.AddObjectFilter(
		this, GET_MEMBER_NAME_CHECKED(UBTTask_Attack, TargetKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	AMonsterBase* Monster = Cast<AMonsterBase>(AICon->GetPawn());
	if (!Monster) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!Target) return EBTNodeResult::Failed;

	// 사거리 밖이면 실패 → Sequence가 중단되고 다시 MoveTo부터
	const float Dist = FVector::Dist(
		Monster->GetActorLocation(), Target->GetActorLocation());

	if (Dist > Monster->GetAttackTriggerRange())
	{
		return EBTNodeResult::Failed;
	}

	Monster->TryAttack(Target);
	return EBTNodeResult::Succeeded;
}