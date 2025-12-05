// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DXPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDX_API ADXPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ADXPlayerState();
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsCop;
	
};
