// Copyright 2022-2022 Josh Villyat. All Rights Reserved


#include "SavingUtils.h"

#include "PuzzleSaveGame.h"
#include "Kismet/GameplayStatics.h"

int SavingUtils::GetSavedLevel()
{
	if(UGameplayStatics::DoesSaveGameExist("Save", 0))
		return Cast<UPuzzleSaveGame>(UGameplayStatics::LoadGameFromSlot("Save", 0))->LastLevel;

	return Cast<UPuzzleSaveGame>(UGameplayStatics::CreateSaveGameObject(UPuzzleSaveGame::StaticClass()))->LastLevel;
}

void SavingUtils::SetSavedLevel(int NewLevel)
{
	UPuzzleSaveGame* SaveGame = Cast<UPuzzleSaveGame>(UGameplayStatics::CreateSaveGameObject(UPuzzleSaveGame::StaticClass()));
	SaveGame->LastLevel = NewLevel;
	UGameplayStatics::SaveGameToSlot(SaveGame, "Save", 0);
}

TArray<FTransform> SavingUtils::GetSpawnLocations()
{
	int SavedLevel = GetSavedLevel();
	FTransform SpawnA;
	FTransform SpawnB;
	
	switch(SavedLevel)
	{
	case 1: // The two spawn points for level 1
		SpawnA = FTransform(FRotator(0, -180, 0), FVector(-940.0, 1853.0, 257.625885), FVector(1));
		SpawnB = FTransform(FRotator(0, -180, 0), FVector(-940.0, 1000, 257.625885), FVector(1));
		break;
	default:
		break;
	}
	
	TArray<FTransform> ReturnedArray;
	ReturnedArray.Add(SpawnA);
	ReturnedArray.Add(SpawnB);
	return ReturnedArray;
}