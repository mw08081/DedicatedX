// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/BTTask_SetRandomLocation.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"

UBTTask_SetRandomLocation::UBTTask_SetRandomLocation()
{
	NodeName = FString(TEXT("SetRandomLocation"));
}

EBTNodeResult::Type UBTTask_SetRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* OwnerBlackboard = OwnerComp.GetBlackboardComponent();
	if (OwnerBlackboard == nullptr) return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (NavSystem == nullptr) return EBTNodeResult::Failed;

	FNavLocation LOC;
	NavSystem->GetRandomPoint(LOC);

	//DrawDebugSphere(GetWorld(), LOC.Location, 25, 25, FColor::Red, true);
	OwnerBlackboard->SetValueAsVector(GetSelectedBlackboardKey(), LOC.Location);

	return EBTNodeResult::Succeeded;
}
