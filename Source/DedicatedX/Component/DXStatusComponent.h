// DXStatusComponent.h

#pragma once

#include "Components/ActorComponent.h"
#include "DXStatusComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentHPChangedDelegate, float /*InCurrentHP*/);
DECLARE_MULTICAST_DELEGATE(FOnOutOfCurrentHPDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMaxHPChangedDelegate, float /*InMaxHP*/);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEDICATEDX_API UDXStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDXStatusComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	float ApplyDamage(float InDamage);

	float GetCurrentHP() const { return CurrentHP; }
	void SetCurrentHP(float InCurrentHP);

	float GetMaxHP() const { return MaxHP; }
	void SetMaxHP(float InMaxHP);

	UFUNCTION()
	void OnRep_CurrentHP();

public:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHP)
	float CurrentHP;


	UPROPERTY(Replicated)
	float MaxHP;

	FOnCurrentHPChangedDelegate OnCurrentHPChanged;

	FOnOutOfCurrentHPDelegate OnOutOfCurrentHP;

	FOnMaxHPChangedDelegate OnMaxHPChanged;

};
