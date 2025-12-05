// DXGameModeBase.h

#pragma once

#include "GameFramework/GameModeBase.h"
#include "DXGameModeBase.generated.h"

class ADXPlayerController;

/**
 *
 */
UCLASS()
class DEDICATEDX_API ADXGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnMainTimerElapsed();

public:
	FTimerHandle MainTimerHandle;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<ADXPlayerController>> AlivePlayerControllers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<ADXPlayerController>> DeadPlayerControllers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WaitingTime = 5;

	int32 RemainWaitingTimeForPlaying = 5;

	int32 MinimumPlayerCountForPlaying = 2;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EndingTime = 15;

	int32 RemainWaitingTimeForEnding = 15;

public:
	void OnCharacterDead(ADXPlayerController* InController);
	
private:
	void SetPlayerRole();
	void OnGameEnd();
	void NotifyToAllPlayer(const FString& NotificationString);


};
