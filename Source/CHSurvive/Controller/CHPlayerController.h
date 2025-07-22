// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "CHPlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class ACHPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACHPlayerController();

	virtual void BeginPlay();

	// /** Time Threshold to know if it was a short press */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	// float ShortPressThreshold;

// 	/** FX Class that we will spawn when clicking */
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
// 	UNiagaraSystem* FXCursor;
//
//
// private:
// 	/** MappingContext */
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
// 	UInputMappingContext* DefaultMappingContext;
// 	
//
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
// 	UInputAction* ClickAction;
//
// 	
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
// 	UInputAction* SprintAction;
//
// 	
// 	
// protected:
// 	/** True if the controlled character should navigate to the mouse cursor. */
// 	uint32 bMoveToMouseCursor : 1;
//
// 	void Sprint();
// 	void SprintEnd();
// 	virtual void SetupInputComponent() override;
// 	
// 	// To add mapping context
// 	virtual void BeginPlay();
//
// 	/** Input handlers for SetDestination action. */
// 	void OnClickMove();
// 	

private:
	//FVector CachedDestination;
};


