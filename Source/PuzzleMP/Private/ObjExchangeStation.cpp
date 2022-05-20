// Copyright 2022-2022 Josh Villyat. All Rights Reserved


#include "ObjExchangeStation.h"

#include "PickupInterface.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "PuzzleMP/MyCharacter.h"
#include "PuzzleMP/Trigger.h"

// Sets default values
AObjExchangeStation::AObjExchangeStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	BodyMeshes.Add(CreateDefaultSubobject<UStaticMeshComponent>(FName("BodyMesh01")));
	BodyMeshes.Add(CreateDefaultSubobject<UStaticMeshComponent>(FName("BodyMesh02")));
	for(UStaticMeshComponent* BodyMesh : BodyMeshes) BodyMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	Triggers.Add(CreateDefaultSubobject<UBoxComponent>(FName("Trigger01")));
	Triggers.Add(CreateDefaultSubobject<UBoxComponent>(FName("Trigger02")));
	for(int x = 0; x < Triggers.Num(); x++)
	{
		if(BodyMeshes[x]) Triggers[x]->AttachToComponent(BodyMeshes[x], FAttachmentTransformRules::KeepRelativeTransform);
		Triggers[x]->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		Triggers[x]->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	}
		
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Meshes/General/SM_ObjExchangeStation.SM_ObjExchangeStation"));
	if(MeshAsset.Succeeded()) for(UStaticMeshComponent* BodyMesh : BodyMeshes) BodyMesh->SetStaticMesh(MeshAsset.Object);

	BodyMeshes[1]->SetRelativeLocation(FVector(-142, 0, 0));
	BodyMeshes[1]->SetRelativeRotation(FRotator(0, 180, 0));

	ItemLocations.Add(CreateDefaultSubobject<USceneComponent>(FName("ItemHolder01")));
	ItemLocations.Add(CreateDefaultSubobject<USceneComponent>(FName("ItemHolder02")));
	for(int x = 0; x < ItemLocations.Num(); x++)
	{
		ItemLocations[x]->AttachToComponent(Triggers[x], FAttachmentTransformRules::KeepRelativeTransform);
		ItemLocations[x]->SetRelativeScale3D(FVector(0.4));
	}

	MovingItemHolders.Add(CreateDefaultSubobject<USceneComponent>(FName("MovingItemHolder01")));
	MovingItemHolders.Add(CreateDefaultSubobject<USceneComponent>(FName("MovingItemHolder02")));
	for(int x = 0; x < MovingItemHolders.Num(); x++)
		MovingItemHolders[x]->AttachToComponent(ItemLocations[x], FAttachmentTransformRules::KeepRelativeTransform);
}


// Called every frame
void AObjExchangeStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!HasAuthority()) return;
	
	// Removes item from HeldItems if picked up by another actor
	for(const TPair<int, AActor*> HeldItem : HeldItems)
	{
		const int SideIndex = HeldItem.Key;
		const AActor* HeldActor = HeldItem.Value;
		if(!HeldActor) HeldItems.Remove(SideIndex);
		if(HeldActor->GetAttachParentActor() != this)
		{
			HeldItems.Remove(SideIndex);
			UKismetSystemLibrary::PrintString(GetWorld(), TEXT("ObjExchangeStation - Removed held item"), true, true, FColor::Blue, 2);
			break; // Stop the for-loop here, until next tick, as HeldItems array was adjusted
		}
	}
	
	// Only does this check if needed | Saves CPU
	if(HeldItems.Num() < 2) { for(int x = 0; x < Triggers.Num(); x++) TriggerCheck(x); }
	
	// Moves alpha closer to TargetAlpha (0 or 1) | Clamps the value within that range just in-case target alpha exceeds the expected values
	Alpha = FMath::Clamp(TargetAlpha == 0 ? Alpha - DeltaTime : Alpha + DeltaTime, 0.0f, 1.0f);

	if(TargetAlpha == Alpha)
	{
		if(Alpha == 0.0f) return; // Only continue if the alpha is actually changing; otherwise, waste of CPU
		if(Alpha == 1.0f)
		{
			for(const TPair<int, AActor*> HeldItem : HeldItems)
			{
				const int Index = HeldItem.Key;
				AActor* HeldActor = HeldItem.Value;
				const IPickupInterface* PickupInterface = Cast<IPickupInterface>(HeldActor);
				PickupInterface->Execute_OnPickup(HeldActor, this, ItemLocations[Index == 1 ? 0 : 1]); // Attach the item to its new side's location
			}
			
			// Move item holders back
			TargetAlpha = 0.0f;
			Alpha = 0.0f;
		}
	}
	
	for(int x = 0; x < MovingItemHolders.Num(); x++)
	{
		USceneComponent* Holder = MovingItemHolders[x];
		FVector OriginalLocation = ItemLocations[x]->GetComponentLocation();
		FVector NewLocation = ItemLocations[x == 0 ? 1 : 0]->GetComponentLocation();
		const FVector Location = FMath::Lerp(OriginalLocation, NewLocation, Alpha);
		SetHolderLocation(Holder, Location);
	}
}

// Multicast function (executed for every machine) that updates the location of the specified item holder
void AObjExchangeStation::SetHolderLocation_Implementation(USceneComponent* Holder, FVector NewLocation)
{
	Holder->SetWorldLocation(NewLocation);
}


void AObjExchangeStation::TriggerCheck(int TriggerIndex)
{
	const UBoxComponent* Trigger = Triggers[TriggerIndex];
	if(!Trigger) return;

	if(HeldItems.Find(TriggerIndex)) return; // If this side is occupied, don't check for other actors
	
	TArray<AActor*> OverlappingActors;
	Trigger->GetOverlappingActors(OverlappingActors);
	for(AActor* Actor : OverlappingActors)
	{
		if(!ObjectFilter.Contains(Actor->GetClass())) continue;
		if(Actor->GetAttachParentActor() != nullptr) continue;
		const IPickupInterface* PickupInterface = Cast<IPickupInterface>(Actor);
		if(!PickupInterface) continue;
		USceneComponent* ItemLocation = ItemLocations[TriggerIndex];
		ItemLocation->SetRelativeScale3D(*ObjectFilter.Find(Actor->GetClass()));
		PickupInterface->Execute_OnPickup(Actor, this, ItemLocation); // Attaches the actor to the appropriate location
		HeldItems.Add(TriggerIndex, Actor);
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("ObjExchangeStation - Actor now in station"), true, true, FColor::Blue, 2);
		if(HeldItems.Num() >= 2) SetupItemsForMovement();
	}
}

void AObjExchangeStation::SetupItemsForMovement()
{
	for(TPair<int, AActor*> HeldItem : HeldItems)
	{
		int Index = HeldItem.Key;
		AActor* HeldActor = HeldItem.Value;

		const IPickupInterface* PickupInterface = Cast<IPickupInterface>(HeldActor);
		PickupInterface->Execute_OnPickup(HeldActor, this, MovingItemHolders[Index]);
	}
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("ObjExchangeStation - Swap actors"), true, true, FColor::Blue, 2);
	TargetAlpha = 1.0f;
}