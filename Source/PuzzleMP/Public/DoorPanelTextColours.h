// Copyright 2022-2022 Josh Villyat. All Rights Reserved

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class DoorPanelTextColours : uint8
{
	TC_Heading		UMETA(DisplayName="Heading"),
	TC_Subheading		UMETA(DisplayName="Subheading"),
	TC_Alert		UMETA(DisplayName="Alert"),
};