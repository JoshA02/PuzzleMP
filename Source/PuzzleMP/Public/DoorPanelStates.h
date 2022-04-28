// Copyright 2022-2022 Josh Villyat. All Rights Reserved

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class DoorPanelStates : uint8
{
	DS_Begin		UMETA(DisplayName="BEGIN"),
	DS_2PlayersNeeded		UMETA(DisplayName="2 PLAYERS NEEDED"),
	DS_Settings		UMETA(DisplayName="SETTINGS"),
	DS_EnterToStart		UMETA(DisplayName="PRESS ENTER TO START"),
	DS_Quit		UMETA(DisplayName="QUIT"),
	DS_None		UMETA(DisplayName="{NONE}"),
};