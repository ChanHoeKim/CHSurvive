// Copyright Epic Games, Inc. All Rights Reserved.

#include "CHGameMode.h"
#include "Controller/CHPlayerController.h"
#include "Character/CHPlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "CHGameState.h"

ACHGameMode::ACHGameMode()
{
	//GameMode에 GameState를 등록
	GameStateClass = ACHGameState::StaticClass();
	
// 	// use our custom PlayerController class
// 	PlayerControllerClass = ATP_TopDownPlayerController::StaticClass();
//
// 	// set default pawn class to our Blueprinted character
// 	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
// 	if (PlayerPawnBPClass.Class != nullptr)
// 	{
// 		DefaultPawnClass = PlayerPawnBPClass.Class;
// 	}
//
// 	// set default controller to our Blueprinted controller
// 	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
// 	if(PlayerControllerBPClass.Class != NULL)
// 	{
// 		PlayerControllerClass = PlayerControllerBPClass.Class;
// 	}

	
}