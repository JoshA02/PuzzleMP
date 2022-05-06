// Copyright 2022-2022 Josh Villyat. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Cube.h"
#include "InteractInterface.h"
#include "PickupInterface.h"
#include "PickupableCube.generated.h"


UCLASS()
class PUZZLEMP_API APickupableCube : public ACube, public IPickupInterface
{
	GENERATED_BODY()

public:
	APickupableCube();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=Interaction)
	void OnPickup(AActor* Caller, USceneComponent* ToAttach);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=Interaction)
	void OnPutdown(AActor* Caller);

private:
	virtual void BeginPlay() override;

	void Pickup(USceneComponent* ToAttach);
	UFUNCTION(NetMulticast, Reliable)
	void PickupMulticast(USceneComponent* ToAttach);

	void Putdown();
	UFUNCTION(NetMulticast, Reliable)
	void PutdownMulticast();
};
