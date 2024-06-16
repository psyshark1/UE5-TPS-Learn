// Copyright Epic Games, Inc. All Rights Reserved.

#include "tps_lrnGameMode.h"
#include "tps_lrnCharacter.h"
#include "UObject/ConstructorHelpers.h"

Atps_lrnGameMode::Atps_lrnGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
