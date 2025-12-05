// DXPlayerCharacter.cpp


#include "DXPlayerCharacter.h"

#include "DedicatedX.h"

#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"

#include "GameFramework/GameStateBase.h"
#include "EngineUtils.h"
#include "Game/DXGameStateBase.h"
#include "Component/DXStatusComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DxPlayerController.h"
#include "Player/DXPlayerState.h"

ADXPlayerCharacter::ADXPlayerCharacter()
	: bCanAttack(true)
	, MeleeAttackMontagePlayTime(0.f)
	, MinAllowedTimeForMeleeAttack(0.02f)
{
	PrimaryActorTick.bCanEverTick = false;

	DX_LOG_NET(LogDXNet, Log, TEXT(""));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	StatusComponent = CreateDefaultSubobject<UDXStatusComponent>(TEXT("StatusComponent"));
}


void ADXPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADXPlayerCharacter::HandleMoveInput);

	EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADXPlayerCharacter::HandleLookInput);

	EIC->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

	EIC->BindAction(MeleeAttackAction, ETriggerEvent::Started, this, &ThisClass::HandleMeleeAttackInput);
}

void ADXPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled() == true)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		checkf(IsValid(PC) == true, TEXT("PlayerController is invalid."));

		UEnhancedInputLocalPlayerSubsystem* EILPS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		checkf(IsValid(EILPS) == true, TEXT("EnhancedInputLocalPlayerSubsystem is invalid."));

		EILPS->AddMappingContext(InputMappingContext, 0);
	}

	if (IsValid(MeleeAttackMontage) == true)
	{
		MeleeAttackMontagePlayTime = MeleeAttackMontage->GetPlayLength();
	}

	StatusComponent->OnOutOfCurrentHP.AddUObject(this, &ThisClass::OnDeath);
}

void ADXPlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, bCanAttack);
}

void ADXPlayerCharacter::HandleMoveInput(const FInputActionValue& InValue)
{
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Controller is invalid."));
		return;
	}

	const FVector2D InMovementVector = InValue.Get<FVector2D>();

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator ControlYawRotation(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, InMovementVector.X);
	AddMovementInput(RightDirection, InMovementVector.Y);
}

void ADXPlayerCharacter::HandleLookInput(const FInputActionValue& InValue)
{
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Controller is invalid."));
		return;
	}

	const FVector2D InLookVector = InValue.Get<FVector2D>();

	AddControllerYawInput(InLookVector.X);
	AddControllerPitchInput(InLookVector.Y);
}

void ADXPlayerCharacter::HandleMeleeAttackInput(const FInputActionValue& InValue)
{
	ADXPlayerState* DXPlayerState = GetPlayerState<ADXPlayerState>();
	if (IsValid(DXPlayerState) == true)
	{
		if (DXPlayerState->bIsCop == true && true == bCanAttack && GetCharacterMovement()->IsFalling() == false)
		{
			ServerRPCMeleeAttack();

			if (HasAuthority() == false && IsLocallyControlled() == true)
			{
				PlayMeleeAttackMontage();
			}
		}
	}
}


float ADXPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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

void ADXPlayerCharacter::CheckMeleeAttackHit()
{
	if (IsLocallyControlled() == true)
	{
		TArray<FHitResult> OutHitResults;
		TSet<ACharacter*> DamagedCharacters;
		FCollisionQueryParams Params(NAME_None, false, this);

		const float MeleeAttackRange = 50.f;
		const float MeleeAttackRadius = 50.f;
		//const float MeleeAttackDamage = 10.f;
		const FVector Forward = GetActorForwardVector();
		const FVector Start = GetActorLocation() + Forward * GetCapsuleComponent()->GetScaledCapsuleRadius();
		const FVector End = Start + GetActorForwardVector() * MeleeAttackRange;

		bool bIsHitDetected = GetWorld()->SweepMultiByChannel(OutHitResults, Start, End, FQuat::Identity, ECC_Camera, FCollisionShape::MakeSphere(MeleeAttackRadius), Params);
		if (bIsHitDetected == true)
		{
			for (auto const& OutHitResult : OutHitResults)
			{
				ACharacter* DamagedCharacter = Cast<ACharacter>(OutHitResult.GetActor());
				if (IsValid(DamagedCharacter) == true)
				{
					DamagedCharacters.Add(DamagedCharacter);
				}
			}

			FDamageEvent DamageEvent;
			for (auto const& DamagedCharacter : DamagedCharacters)
			{
				//DamagedCharacter->TakeDamage(MeleeAttackDamage, DamageEvent, GetController(), this);
				ServerRPCPerformMeleeHit(DamagedCharacter, GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
			}
		}

		FColor DrawColor = bIsHitDetected ? FColor::Green : FColor::Red;
		DrawDebugMeleeAttack(DrawColor, Start, End, Forward);
	}
}

void ADXPlayerCharacter::ServerRPCPerformMeleeHit_Implementation(ACharacter* InDamagedCharacters, float InCheckTime)
{
	if (IsValid(InDamagedCharacters) == true)
	{
		const float MeleeAttackDamage = 10.f;
		FDamageEvent DamageEvent;
		InDamagedCharacters->TakeDamage(MeleeAttackDamage, DamageEvent, GetController(), this);
	}
}

bool ADXPlayerCharacter::ServerRPCPerformMeleeHit_Validate(ACharacter* InDamagedCharacters, float InCheckTime)
{
	return true;
}


void ADXPlayerCharacter::ServerRPCMeleeAttack_Implementation()
{
	if (HasAuthority() == true)
	{
		bCanAttack = false;

		OnRep_CanAttack();

		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]()
			{
				bCanAttack = true;
				OnRep_CanAttack();
			}), MeleeAttackMontagePlayTime, false);
	}

	PlayMeleeAttackMontage();

	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (IsValid(PlayerController) == true && GetController() != PlayerController)  // 이 캐릭터는 공격한 플레이어의 캐릭터임. 공격한 플레이어의 컨트롤러 외의 컨트롤러들을 찾기 위한 조건문.
		{
			ADXPlayerCharacter* OtherPlayerCharacter = Cast<ADXPlayerCharacter>(PlayerController->GetPawn());
			if (OtherPlayerCharacter)
			{
				OtherPlayerCharacter->ClientRPCPlayMeleeAttackMontage(this); // 다른 플레이어 컨트롤러의 캐릭터에 공격한 클라이언트의 캐릭터를 넘겨줘서, 애니메이션이 재생되게끔 함.
			}
		}
	}
}

bool ADXPlayerCharacter::ServerRPCMeleeAttack_Validate()
{
	return true;
}

void ADXPlayerCharacter::DrawDebugMeleeAttack(const FColor& DrawColor, FVector TraceStart, FVector TraceEnd, FVector Forward)
{
	const float MeleeAttackRange = 50.f;
	const float MeleeAttackRadius = 50.f;
	FVector CapsuleOrigin = TraceStart + (TraceEnd - TraceStart) * 0.5f;
	float CapsuleHalfHeight = MeleeAttackRange * 0.5f;
	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, MeleeAttackRadius, FRotationMatrix::MakeFromZ(Forward).ToQuat(), DrawColor, false, 5.0f);
}

void ADXPlayerCharacter::MulticastRPCMeleeAttack_Implementation()
{
	if (HasAuthority() == false && IsLocallyControlled() == false)
	{
		PlayMeleeAttackMontage();
	}
}

void ADXPlayerCharacter::OnRep_CanAttack()
{
	if (bCanAttack == true)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_None);
	}
}

void ADXPlayerCharacter::ClientRPCPlayMeleeAttackMontage_Implementation(ADXPlayerCharacter* InTargetCharacter)
{
	if (IsValid(InTargetCharacter) == true)
	{
		InTargetCharacter->PlayMeleeAttackMontage();
	}
}

void ADXPlayerCharacter::PlayMeleeAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance) == true)
	{
		AnimInstance->StopAllMontages(0.f);
		AnimInstance->Montage_Play(MeleeAttackMontage);
	}
}

void ADXPlayerCharacter::OnDeath()
{
	ADXPlayerController* PlayerController = GetController<ADXPlayerController>();
	if (IsValid(PlayerController) == true && HasAuthority() == true)
	{
		PlayerController->OnCharacterDead();
	}
}