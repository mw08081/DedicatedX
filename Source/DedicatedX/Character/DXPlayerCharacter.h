// DXPlayerCharacter.h

#pragma once

#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "DXPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UDXStatusComponent;

UCLASS()
class DEDICATEDX_API ADXPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region ACharacter Override

public:
	ADXPlayerCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

#pragma endregion

#pragma region DXPlayerCharacter Components

public:
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }

	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DXPlayerCharacter|Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DXPlayerCharacter|Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DXPlayerCharacter|Components")
	TObjectPtr<UDXStatusComponent> StatusComponent;

#pragma endregion

#pragma region Input

private:
	void HandleMoveInput(const FInputActionValue& InValue);

	void HandleLookInput(const FInputActionValue& InValue);

	void HandleMeleeAttackInput(const FInputActionValue& InValue);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DXPlayerCharacter|Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DXPlayerCharacter|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DXPlayerCharacter|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DXPlayerCharacter|Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DXPlayerCharacter|Input")
	TObjectPtr<UInputAction> MeleeAttackAction;

#pragma endregion

#pragma region Attack

public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void CheckMeleeAttackHit();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCPerformMeleeHit(ACharacter* InDamagedCharacters, float InCheckTime);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCMeleeAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCMeleeAttack();

	UFUNCTION()
	void OnRep_CanAttack();

	void PlayMeleeAttackMontage();

	UFUNCTION(Client, Unreliable)
	void ClientRPCPlayMeleeAttackMontage(ADXPlayerCharacter* InTargetCharacter);

	UFUNCTION()
	void OnDeath();

private:
	void DrawDebugMeleeAttack(const FColor& DrawColor, FVector TraceStart, FVector TraceEnd, FVector Forward);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_CanAttack)
	bool bCanAttack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> MeleeAttackMontage;

	float MeleeAttackMontagePlayTime;

	float MinAllowedTimeForMeleeAttack;

#pragma endregion



};
