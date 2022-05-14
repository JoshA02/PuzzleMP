// Copyright 2022-2022 Josh Villyat. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "ObjExchangeStation.generated.h"

UCLASS()
class PUZZLEMP_API AObjExchangeStation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObjExchangeStation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<UStaticMeshComponent*> BodyMeshes;

	UPROPERTY(EditInstanceOnly)
	TArray<UBoxComponent*> Triggers;
	
	UPROPERTY(EditInstanceOnly, Category=Default)
	TMap<TSubclassOf<AActor>, FVector> ObjectFilter;

	UPROPERTY(EditInstanceOnly)
	TArray<USceneComponent*> ItemHolders;

	UPROPERTY(EditInstanceOnly)
	TArray<USceneComponent*> MovingItemHolders;

	TMap<int, AActor*> HeldItems; // Index | Actor being held

	bool AllowPickup;
	
	float TargetAlpha = 0.0f;
	float Alpha = TargetAlpha;

	void TriggerCheck(int TriggerIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
