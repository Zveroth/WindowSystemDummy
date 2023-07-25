// Copyright Epic Games, Inc. All Rights Reserved.

#include "WindowSystemDummyPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "WindowSystemDummyCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AWindowSystemDummyPlayerController::AWindowSystemDummyPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void AWindowSystemDummyPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void AWindowSystemDummyPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AWindowSystemDummyPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AWindowSystemDummyPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AWindowSystemDummyPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AWindowSystemDummyPlayerController::OnSetDestinationReleased);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &AWindowSystemDummyPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &AWindowSystemDummyPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &AWindowSystemDummyPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &AWindowSystemDummyPlayerController::OnTouchReleased);
	}
}

void AWindowSystemDummyPlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void AWindowSystemDummyPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AWindowSystemDummyPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void AWindowSystemDummyPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void AWindowSystemDummyPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}
