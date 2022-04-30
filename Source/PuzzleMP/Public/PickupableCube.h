// Copyright 2022-2022 Josh Villyat. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Cube.h"
#include "InteractInterface.h"
#include "PickupableCube.generated.h"


UCLASS()
class PUZZLEMP_API APickupableCube : public ACube, public IInteractInterface
{
	GENERATED_BODY()

public:
	APickupableCube();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=Interaction)
	void OnInteract(AActor* Caller);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=Interaction)
	void OnStopInteract(AActor* Caller);

private:
	virtual void BeginPlay() override;

	void Pickup();
	void Putdown();
	
	UFUNCTION(NetMulticast, Reliable)
	void PickupMulticast();
	UFUNCTION(NetMulticast, Reliable)
	void PutdownMulticast();
};
