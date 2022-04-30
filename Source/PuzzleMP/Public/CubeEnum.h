// Copyright 2022-2022 Josh Villyat. All Rights Reserved

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class CubeEnum : uint8
{
	CUBE_DEFAULT		UMETA(DisplayName="Default"),
	CUBE_HOST		UMETA(DisplayName="Host"),
	CUBE_CLIENT		UMETA(DisplayName="Client"),
};