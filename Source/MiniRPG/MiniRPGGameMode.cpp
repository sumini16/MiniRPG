// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiniRPGGameMode.h"
#include "MiniRPGCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMiniRPGGameMode::AMiniRPGGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
