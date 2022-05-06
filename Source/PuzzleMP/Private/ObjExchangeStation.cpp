// Copyright 2022-2022 Josh Villyat. All Rights Reserved


#include "ObjExchangeStation.h"

#include "InteractInterface.h"
#include "PickupInterface.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "PuzzleMP/MyCharacter.h"

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

	ItemHolders.Add(CreateDefaultSubobject<USceneComponent>(FName("ItemHolder01")));
	ItemHolders.Add(CreateDefaultSubobject<USceneComponent>(FName("ItemHolder02")));
	for(int x = 0; x < ItemHolders.Num(); x++)
	{
		ItemHolders[x]->AttachToComponent(Triggers[x], FAttachmentTransformRules::KeepRelativeTransform);
		ItemHolders[x]->SetRelativeScale3D(FVector(0.4));
	}
}

// Called when the game starts or when spawned
void AObjExchangeStation::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void AObjExchangeStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!HasAuthority()) return;
	
	for(int x = 0; x < Triggers.Num(); x++) TriggerCheck(x);
	
/*
	for(TPair<int, AActor*> HeldItem : HeldItems)
	{
		AActor* Actor = HeldItem.Value;
		if( !Actor->GetAttachParentActor() || Actor->GetAttachParentActor() != this)
		{
			UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Not attached to this. Remove from HeldItems"), true, true, FColor::Blue, 2);
			HeldItems.Remove(HeldItem.Key); // Item is no longer attached to the station, remove it from HeldItems
			continue;
		}
		// SetCollisionForActor(Actor, AllowPickup);
		// SetCollisionForActor_Multicast(Actor, AllowPickup);
	}*/

	
	AllowPickup = TargetAlpha == Alpha; // Allow pickup when the actors are NOT being transferred
	// UKismetSystemLibrary::PrintString(GetWorld(), (TEXT("AllowPickup: %s"), *FString::SanitizeFloat(AllowPickup)), true, true, FColor::Blue, 2);
	
	
	if(TargetAlpha > Alpha) Alpha = FMath::Clamp(Alpha = Alpha + DeltaTime, 0.0f, 1.0f);
	if(TargetAlpha < Alpha) Alpha = FMath::Clamp(Alpha = Alpha - DeltaTime, 0.0f, 1.0f);
	
	if(HeldItems.Num() < 2) return;

	// Swapping...
	for(int x = 0; x < BodyMeshes.Num(); x++)
	{
		const int OtherX = (x == 0 ? 1 : 0);
		FVector NewLocation = FMath::Lerp(ItemHolders[x]->GetComponentLocation(), ItemHolders[OtherX]->GetComponentLocation(), Alpha);
		AActor* Actor = *HeldItems.Find(x);
		Actor->SetActorLocation(NewLocation);
	}
}

void AObjExchangeStation::SetCollisionForActor_Multicast_Implementation(AActor* Actor, bool CollisionEnabled) { SetCollisionForActor(Actor, CollisionEnabled); }
void AObjExchangeStation::SetCollisionForActor(AActor* Actor, bool CollisionEnabled) { Actor->SetActorEnableCollision(CollisionEnabled); }



void AObjExchangeStation::TriggerCheck(int TriggerIndex)
{
	if(HeldItems.Find(TriggerIndex)) return; // If an item is already being held in this side, don't do anything
	
	const UBoxComponent* Trigger = Triggers[TriggerIndex];
	TArray<TSubclassOf<AActor>> AllowedClasses;
	ObjectFilter.GetKeys(AllowedClasses);
	
	TArray<AActor*> OverlappingActors;
	Trigger->GetOverlappingActors(OverlappingActors);
	if(OverlappingActors.Num() <= 0) return;
	
	for(AActor* OverlappingActor : OverlappingActors)
	{
		if(!ObjectFilter.Contains(OverlappingActor->GetClass())) continue; // Ensure the item's contained in the filter TMap

		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Item placed in exchange"), true, true, FColor::Blue, 2);

		// If the item is 'pickupable', execute the OnTrigger function to trigger the Pickup functionality
		IPickupInterface* PickupInterface = Cast<IPickupInterface>(OverlappingActor);
		PickupInterface->Execute_OnPickup(OverlappingActor, this);
		
		OverlappingActor->AttachToComponent(ItemHolders[TriggerIndex], FAttachmentTransformRules::SnapToTargetIncludingScale); // Snap actor to scene component
		const FVector* DesiredScale = ObjectFilter.Find(OverlappingActor->GetClass()); // Get the actor's desired exchange scale
		ItemHolders[TriggerIndex]->SetRelativeScale3D(*DesiredScale); // Set the scale
		HeldItems.Add(TriggerIndex, OverlappingActor);

		if(HeldItems.Num() >= 2) TargetAlpha = 1.0;
	}
}