// Copyright 2022-2022 Josh Villyat. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "InteractInterface.h"
#include "GameFramework/Actor.h"
#include "PowerThreshold.generated.h"

UCLASS()
class PUZZLEMP_API APowerThreshold : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerThreshold();

protected:
	UPROPERTY(EditInstanceOnly, Category=Default)
	int RequiredPowerSources = 4;

	int CurrentPowerSources = 0;

	UPROPERTY(EditInstanceOnly, Category=Default)
	TArray<AActor*> ActorsToPower;

public:	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=Interaction)
	void OnInteract(AActor* Caller);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=Interaction)
	void OnStopInteract(AActor* Caller);

};
