// DXPlayerController.h

#pragma once

#include "GameFramework/PlayerController.h"
#include "DXPlayerController.generated.h"

class UUserWidget;

/**
 *
 */
UCLASS()
class DEDICATEDX_API ADXPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	FText NotificationText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUserWidget> NotificationTextUIClass;

public:
	void OnCharacterDead();

#pragma region EndGame

public:

	UFUNCTION(Client, Reliable)
	void ClientRPCShowGameResultWidget(int32 InRanking);

	UFUNCTION(Client, Reliable)
	void ClientRPCReturnToTitle();

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UUW_GameResult> GameResultUIClass;

	UPROPERTY()
	TObjectPtr<class UUW_GameResult> GameResultUIInstance;

#pragma endregion


#pragma region EndGame

public:

	//UFUNCTION(Client, Reliable)
	//void ClientRPCShowGameResultWidget(int32 InRanking);

	//UFUNCTION(Client, Reliable)
	//void ClientRPCReturnToTitle();

private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> HUDClass;
public:
	UPROPERTY()
	TObjectPtr<class UUserWidget> HUDInstance;

#pragma endregion

};
