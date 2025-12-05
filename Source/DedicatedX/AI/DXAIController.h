// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DXAIController.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDX_API ADXAIController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* InPawn) override;

public:
	UPROPERTY(EditDefaultsOnly)
	class UBehaviorTree* BehaviorTreeComp = nullptr;

public:
	void OnCharacterDead();
	
};
