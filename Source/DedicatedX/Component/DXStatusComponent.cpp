// DXStatusComponent.cpp


#include "Component/DXStatusComponent.h"
#include "Net/UnrealNetwork.h"

UDXStatusComponent::UDXStatusComponent()
	: CurrentHP(30.0f)
	, MaxHP(30.f)
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UDXStatusComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentHP);
	DOREPLIFETIME(ThisClass, MaxHP);
}

float UDXStatusComponent::ApplyDamage(float InDamage)
{
	const float PreviousHP = CurrentHP;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, PreviousHP);

	SetCurrentHP(PreviousHP - ActualDamage);

	return ActualDamage;
}

void UDXStatusComponent::SetCurrentHP(float InCurrentHP)
{
	CurrentHP = InCurrentHP;
	if (CurrentHP <= KINDA_SMALL_NUMBER)
	{
		CurrentHP = 0.f;
		OnOutOfCurrentHP.Broadcast();
	}
	OnCurrentHPChanged.Broadcast(CurrentHP);
}

void UDXStatusComponent::SetMaxHP(float InMaxHP)
{
	MaxHP = InMaxHP;

	if (MaxHP < KINDA_SMALL_NUMBER)
	{
		MaxHP = 0.1f;
	}

	OnMaxHPChanged.Broadcast(MaxHP);
}

/// <summary>
/// 리플리케이트되어 값이 변경될때(server->client)
/// 클라이언트의 이벤트가 브로드케스팅 된다
/// </summary>
void UDXStatusComponent::OnRep_CurrentHP()
{
	OnCurrentHPChanged.Broadcast(CurrentHP);
}
