// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DXAICharacter.generated.h"

UCLASS()
class DEDICATEDX_API ADXAICharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region ACharacter Override

public:
	ADXAICharacter();

	virtual void BeginPlay() override;

#pragma endregion

#pragma region DXPlayerCharacter Components

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DXPlayerCharacter|Components")
	TObjectPtr<class UDXStatusComponent> StatusComponent;

#pragma endregion

#pragma region Attack

public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(Client, Unreliable)
	void ClientRPCPlayMeleeAttackMontage(class ADXPlayerCharacter* InTargetCharacter);

	UFUNCTION()
	void OnDeath();

#pragma endregion
};
