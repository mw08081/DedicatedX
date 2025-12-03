// DXBox.cpp


#include "DXBox.h"

#include "Net/UnrealNetwork.h"
#include "DedicatedX.h"


ADXBox::ADXBox()
	: ServerRotationYaw(0.0f)
	, RotationSpeed(30.0f)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(SceneRoot);
	Mesh->SetRelativeLocation(FVector(-50.f, -50.f, 50.f));
}

void ADXBox::BeginPlay()
{
	Super::BeginPlay();

	DX_LOG_ROLE(LogDXNet, Log, TEXT(""));
}

void ADXBox::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ServerRotationYaw);
}

void ADXBox::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority() == true)
	{
		AddActorLocalRotation(FRotator(0.f, RotationSpeed * DeltaSeconds, 0.f));
		ServerRotationYaw = RootComponent->GetComponentRotation().Yaw;
	}
}

void ADXBox::OnRep_ServerRotationYaw()
{
	DX_LOG_NET(LogDXNet, Log, TEXT("OnRep_ServerRotationYaw(): %f"), ServerRotationYaw);

	SetActorRotation(FRotator(0.f, ServerRotationYaw, 0.f));
}
