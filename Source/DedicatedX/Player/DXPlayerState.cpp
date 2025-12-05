// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DXPlayerState.h"
#include "Net/UnrealNetwork.h"

ADXPlayerState::ADXPlayerState()
{
	bReplicates = true;
}

void ADXPlayerState::BeginPlay()
{
	bIsCop = false;
}

void ADXPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsCop);
}
