// Copyright 2022-2022 Josh Villyat. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleMP/Trigger.h"
#include "StandingButton.generated.h"

UCLASS()
class PUZZLEMP_API AStandingButton : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStandingButton();

	UPROPERTY(EditInstanceOnly, Category=Default)
	UStaticMeshComponent* ButtonMesh;

	UPROPERTY(EditInstanceOnly, Category=Default)
	USceneComponent* TriggerLocation;
	
	ATrigger* Trigger;

	UPROPERTY(EditInstanceOnly, Category=Default)
	AActor* ActorToTrigger;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnButtonPressed(AActor* TriggeringActor, AActor* TriggerActor);

};
