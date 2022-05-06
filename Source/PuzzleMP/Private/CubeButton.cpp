// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeButton.h"

#include "Cube.h"
#include "InteractInterface.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACubeButton::ACubeButton()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Button Mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Meshes/General/SM_CubeButton.SM_CubeButton"));
	if(MeshAsset.Succeeded()) Mesh->SetStaticMesh(MeshAsset.Object);
	RootComponent = Mesh;
	
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(FName("Trigger Box"));
	TriggerBox->SetBoxExtent(FVector(5, 5, 10));
	TriggerBox->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	TriggerBox->SetRelativeLocation(FVector(0, 0, 10));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetCollisionObjectType(ECC_Pawn);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACubeButton::TriggerOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ACubeButton::TriggerStop);
}

void ACubeButton::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME( ACubeButton, State );
}

void ACubeButton::TriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(!HasAuthority()) return; // Don't execute for non-server clients
	if(RequiredPresserType && !OtherActor->GetClass()->IsChildOf(RequiredPresserType)) return; // Don't continue if actor doesn't meet requirements
	if(RequiredPresserTag.GetStringLength() > 0 && !RequiredPresserTag.IsEqual(FName("None"), ENameCase::IgnoreCase) && !OtherActor->ActorHasTag(RequiredPresserTag)) return; // Don't continue if actor doesn't have required tag
	State = true;
	ReflectStateChange(); // Reflect the state change for the server
}

void ACubeButton::TriggerStop(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!HasAuthority()) return; // Don't execute for non-server clients
	if(RequiredPresserType && !OtherActor->GetClass()->IsChildOf(RequiredPresserType)) return; // Don't continue if actor doesn't meet requirements
	if(RequiredPresserTag.GetStringLength() > 0 && !RequiredPresserTag.IsEqual(FName("None"), ENameCase::IgnoreCase) && !OtherActor->ActorHasTag(RequiredPresserTag)) return; // Don't continue if actor doesn't have required tag
	State = false;
	ReflectStateChange(); // Reflect the state change for the server
}

// Auto-executes for clients when State is changed, reflecting the state change for non-server clients
void ACubeButton::OnChangeState() { ReflectStateChange(); }

// Executed by everyone when State is changed
void ACubeButton::ReflectStateChange()
{
	IInteractInterface* Interface = Cast<IInteractInterface>(PoweredActor);
	if(!Interface) return;
	if(State) Interface->Execute_OnInteract(PoweredActor, this);
	else Interface->Execute_OnStopInteract(PoweredActor, this);
}

