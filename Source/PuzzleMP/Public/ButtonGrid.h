// Copyright 2022-2022 Josh Villyat. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "L2_Platform.h"
#include "GameFramework/Actor.h"
#include "PuzzleMP/Trigger.h"
#include "ButtonGrid.generated.h"

UCLASS()
class PUZZLEMP_API AButtonGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AButtonGrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<UMaterialInstanceDynamic*> ButtonMaterials;

	void SetupButtonMaterials();

	// Platforms currently selected to move for this sequence
	TArray<AL2_Platform*> PendingPlatforms;
	int PlatformsDoneMoving = 0;

	TArray<FVector> TriggerPositions;
	TArray<ATrigger*> Triggers;

	UPROPERTY(EditInstanceOnly)
	TArray<AL2_Platform*> Platforms;

	UFUNCTION()
	void OnButtonPressed(AActor* TriggeringActor, AActor* TriggerActor);

	AL2_Platform* GetPlatformByNumber(int Number);

	void SetButtonState(int ButtonIndex, bool bNewState);
	UFUNCTION(NetMulticast, Reliable)
	void SetButtonStateMulticast(int ButtonIndex, bool bNewState);

	void StartSequence();

	int SequenceIndex = 0;
	FTimerHandle SequenceHandle;

	UFUNCTION()
	void OnPlatformMoveComplete();

	int PlatformsPending = 0; // The amount of platforms that haven't yet retracted back

	void RandomisePlatformNumbers();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
