// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DXAICharacter.h"

#include "Kismet/GameplayStatics.h"

#include "Game/DXGameStateBase.h"
#include "Character/DXPlayerCharacter.h"
#include "Player/DxPlayerController.h"
#include "AI/DXAIController.h"
#include "Component/DXStatusComponent.h"

ADXAICharacter::ADXAICharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	StatusComponent = CreateDefaultSubobject<UDXStatusComponent>(TEXT("StatusComponent"));
}

void ADXAICharacter::BeginPlay()
{
	Super::BeginPlay();

	StatusComponent->OnOutOfCurrentHP.AddUObject(this, &ThisClass::OnDeath);
}

float ADXAICharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float CurrentHP = StatusComponent->GetCurrentHP();
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	ADXGameStateBase* DXGameState = Cast<ADXGameStateBase>(UGameplayStatics::GetGameState(this));
	if (IsValid(DXGameState) == true && DXGameState->MatchState == EMatchState::Playing)
	{
		StatusComponent->ApplyDamage(ActualDamage);
	}

	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("TakeDamage: %f(%f -> %f)"), DamageAmount, CurrentHP, StatusComponent->GetCurrentHP()), true, true, FLinearColor::Red, 5.f);

	return ActualDamage;
}

void ADXAICharacter::ClientRPCPlayMeleeAttackMontage_Implementation(ADXPlayerCharacter* InTargetCharacter)
{
	if (IsValid(InTargetCharacter) == true)
	{
		InTargetCharacter->PlayMeleeAttackMontage();
	}
}

void ADXAICharacter::OnDeath()
{
	ADXAIController* AIController = GetController<ADXAIController>();
	if (IsValid(AIController) == true && HasAuthority() == true)
	{
		AIController->OnCharacterDead();
	}
}