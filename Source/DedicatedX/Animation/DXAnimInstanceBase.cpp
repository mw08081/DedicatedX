// DXAnimInstanceBase.cpp


#include "Animation/DXAnimInstanceBase.h"

#include "Character/DXPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDXAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ACharacter>(GetOwningActor());
	if (IsValid(OwnerCharacter) == true)
	{
		OwnerCharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
	}
}

void UDXAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(OwnerCharacter) == false || IsValid(OwnerCharacterMovementComponent) == false)
	{
		return;
	}

	Velocity = OwnerCharacterMovementComponent->Velocity;
	GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.f).Size();
	bShouldMove = ((OwnerCharacterMovementComponent->GetCurrentAcceleration().IsNearlyZero()) == false) && (3.f < GroundSpeed);
	bIsFalling = OwnerCharacterMovementComponent->IsFalling();
}

void UDXAnimInstanceBase::AnimNotify_CheckMeleeAttackHit()
{
	if (IsValid(OwnerCharacter) == true)
	{
		ADXPlayerCharacter* ADXOwnerPlayerCharacter = Cast<ADXPlayerCharacter>(OwnerCharacter);
		ADXOwnerPlayerCharacter->CheckMeleeAttackHit();
	}
}
