// Copyright 2022-2022 Josh Villyat. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "MainMenuPlayerCharacter.generated.h"

UCLASS()
class PUZZLEMP_API AMainMenuPlayerCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainMenuPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void NavigateRight();
	void NavigateLeft();
	void Select();

	int PendingActiveSelection = 0;
	int ActiveSelection = 0;

	TArray<FTransform> SelectionLocations;
	
	void SetActiveSelection(int NewSelection);
	
	void OnChangeActiveSelection(int NewSelection);
	

	UPROPERTY(VisibleInstanceOnly)
	UCameraComponent* Camera;

	float CurrentAlpha = 0;

	FTransform TargetTransform;
	FTransform CurrentTransform;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
