// Copyright 2022-2022 Josh Villyat. All Rights Reserved

#pragma once

#include "CoreMinimal.h"

class PUZZLEMP_API SavingUtils
{
public:
	static int GetSavedLevel();
	static void SetSavedLevel(int NewLevel);

	static TArray<FTransform> GetSpawnLocations();
};
