// Copyright 2022-2022 Josh Villyat. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PuzzleSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEMP_API UPuzzleSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	int LastLevel = 3;
};
