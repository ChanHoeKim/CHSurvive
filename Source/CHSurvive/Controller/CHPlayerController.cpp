// Copyright Epic Games, Inc. All Rights Reserved.

#include "CHPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/CHPlayerCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

ACHPlayerController::ACHPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	bEnableClickEvents = true;
	//CachedDestination = FVector::ZeroVector;
}

void ACHPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	FInputModeGameAndUI InputMode;
	//InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	SetInputMode(InputMode);

}

void ACHPlayerController::ServerRPC_PossessShip_Implementation(APawn* ShipPawn)
{
	if (!HasAuthority() || !ShipPawn) return;

	//APawn*

	Possess(ShipPawn);
	//SetViewTargetWithBlend(ShipPawn, 0.25f);
}


// void ACHPlayerController::OnSetDestinationReleased()
// {
// 	// If it was a short press
// 	if (FollowTime <= ShortPressThreshold)
// 	{
// 		// We move there and spawn some particles
// 		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
// 		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
// 	}
//
// 	FollowTime = 0.f;
// }

