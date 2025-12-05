// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DXAIController.h"


#include "BrainComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Game/DXGameModeBase.h"

void ADXAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTreeComp == nullptr) return;

	RunBehaviorTree(BehaviorTreeComp);
}

void ADXAIController::OnCharacterDead()
{
	UBrainComponent* BrainComp = BrainComponent.Get();
	if (BrainComp)
	{
		BrainComp->StopLogic(FString(TEXT("Death")));
	}
}
