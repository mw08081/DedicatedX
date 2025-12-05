// DXGameStateBase.cpp


#include "Game/DXGameStateBase.h"

#include "Net/UnrealNetwork.h"

void ADXGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, AlivePlayerControllerCount);
	DOREPLIFETIME(ThisClass, MatchState);
}
