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

	ItemHolders.Add(CreateDefaultSubobject<USceneComponent>(FName("ItemHolder01")));
	ItemHolders.Add(CreateDefaultSubobject<USceneComponent>(FName("ItemHolder02")));
	for(int x = 0; x < ItemHolders.Num(); x++)
	{
		ItemHolders[x]->AttachToComponent(Triggers[x], FAttachmentTransformRules::KeepRelativeTransform);
		ItemHolders[x]->SetRelativeScale3D(FVector(0.4));
	}

	MovingItemHolders.Add(CreateDefaultSubobject<USceneComponent>(FName("MovingItemHolder01")));
	MovingItemHolders.Add(CreateDefaultSubobject<USceneComponent>(FName("MovingItemHolder02")));
	for(int x = 0; x < MovingItemHolders.Num(); x++)
		MovingItemHolders[x]->AttachToComponent(ItemHolders[x], FAttachmentTransformRules::KeepRelativeTransform);
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
	
	AllowPickup = TargetAlpha == Alpha; // Allow pickup when the actors are NOT being transferred
	// UKismetSystemLibrary::PrintString(GetWorld(), (TEXT("AllowPickup: %s"), *FString::SanitizeFloat(AllowPickup)), true, true, FColor::Blue, 2);
	
	if(TargetAlpha > Alpha) Alpha = FMath::Clamp(Alpha = Alpha + DeltaTime, 0.0f, 1.0f);
	if(TargetAlpha < Alpha) Alpha = FMath::Clamp(Alpha = Alpha - DeltaTime, 0.0f, 1.0f);

	
/*	if(TargetAlpha == 1.0f && Alpha == TargetAlpha && HeldItems.Num() == 0)
	{
		// Reset moving holder locations when neither side has a HeldItem
		TargetAlpha = 0.0f;
		Alpha = 0.0f;
		
		return;
	}*/
	
	if(HeldItems.Num() < 2) return;

	// Swapping...
	for(int x = 0; x < BodyMeshes.Num(); x++)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Swapping..."), true, true, FColor::Blue, 2);

		const int OtherX = (x == 0 ? 1 : 0);
		FVector NewLocation = FMath::Lerp(ItemHolders[x]->GetComponentLocation(), ItemHolders[OtherX]->GetComponentLocation(), Alpha);
		UE_LOG(LogTemp, Log, TEXT("NewLoc: %s"), *NewLocation.ToString());
		MovingItemHolders[x]->SetIsReplicated(true);
		MovingItemHolders[x]->SetWorldLocation(NewLocation);
	}

	for(TPair<int, AActor*> HeldItem : HeldItems)
	{
		if(HeldItem.Value->GetAttachParentActor() != this) HeldItems.Remove(HeldItem.Key);
	}
}

void AObjExchangeStation::TriggerCheck(int TriggerIndex)
{
	UBoxComponent* Trigger = Triggers[TriggerIndex];
	if(!Trigger) return;

	TArray<AActor*> OverlappingActors;
	Trigger->GetOverlappingActors(OverlappingActors);
	for(AActor* Actor : OverlappingActors)
	{
		if(!ObjectFilter.Contains(Actor->GetClass())) return;
		if(Actor->GetAttachParentActor() != nullptr) return;
		UE_LOG(LogTemp, Log, TEXT("Actor in station"));
		IPickupInterface* PickupInterface = Cast<IPickupInterface>(Actor);
		if(!PickupInterface) return;
		USceneComponent* ItemHolder = MovingItemHolders[TriggerIndex];
		PickupInterface->Execute_OnPickup(Actor, this, ItemHolder);
	
		HeldItems.Add(TriggerIndex, Actor);
		if(HeldItems.Num() >= 2) TargetAlpha = 1.0f;
	}
}