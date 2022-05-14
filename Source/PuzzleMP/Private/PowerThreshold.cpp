// Copyright 2022-2022 Josh Villyat. All Rights Reserved


#include "PowerThreshold.h"

// Sets default values
APowerThreshold::APowerThreshold()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void APowerThreshold::OnInteract_Implementation(AActor* Caller)
{
	CurrentPowerSources ++;
	if(CurrentPowerSources >= RequiredPowerSources)
	{
		for(AActor* Actor : ActorsToPower)
		{
			const IInteractInterface* InteractInterface = Cast<IInteractInterface>(Actor);
			InteractInterface->Execute_OnInteract(Actor, this);
		}
	}
}

void APowerThreshold::OnStopInteract_Implementation(AActor* Caller)
{
	CurrentPowerSources --;
	if(CurrentPowerSources < RequiredPowerSources)
	{
		for(AActor* Actor : ActorsToPower)
		{
			const IInteractInterface* InteractInterface = Cast<IInteractInterface>(Actor);
			InteractInterface->Execute_OnStopInteract(Actor, this);
		}
	}
}