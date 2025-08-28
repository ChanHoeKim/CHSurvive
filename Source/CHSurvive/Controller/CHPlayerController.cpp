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
#include "Item/CHShip.h"

ACHPlayerController::ACHPlayerController()
{
	// bShowMouseCursor = true;
	// DefaultMouseCursor = EMouseCursor::Default;
	// bEnableClickEvents = true;
	//CachedDestination = FVector::ZeroVector;
}

void ACHPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (IsLocalController())
	{
		// 커서 보이기 + Game & UI 입력 모드
		bShowMouseCursor = true;
		FInputModeGameAndUI Mode;
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		Mode.SetHideCursorDuringCapture(false); // 클릭해도 커서 숨김 방지
		SetInputMode(Mode);
	}
	
	if (IsLocalController())
	{
		ApplyImcForPawn(GetPawn());
	}
}

void ACHPlayerController::ApplyImcForPawn(APawn* InPawn)
{
	if (!IsLocalController()) return;

	if (ULocalPlayer* LP = GetLocalPlayer())
		if (UEnhancedInputLocalPlayerSubsystem* Sub =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
		{
			// 깔끔하게 모두 비우고 현재 Pawn 타입에 맞는 IMC만 추가
			Sub->ClearAllMappings();
			
			if (DefaultMappingContext)
				Sub->AddMappingContext(DefaultMappingContext, /*Priority*/100);

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
						-1, // Key (고유 ID, -1이면 자동으로 갱신됨)
							5.0f, // Duration (화면에 표시될 시간, 초 단위)
								FColor::Green, // 텍스트 색상
									TEXT("DefaultMappingContext") // 출력할 메시지
									);
			}
		}
}

void ACHPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsLocalController())
	{
		ApplyImcForPawn(InPawn);
		SetViewTargetWithBlend(InPawn, 0.2f);
	}
}
