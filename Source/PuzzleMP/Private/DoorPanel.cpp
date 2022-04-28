// Copyright 2022-2022 Josh Villyat. All Rights Reserved


#include "DoorPanel.h"

#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADoorPanel::ADoorPanel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    UStaticMeshComponent* PanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Panel Mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Game/Meshes/General/SM_DoorPanel_Large.SM_DoorPanel_Large"));
	if(CubeAsset.Succeeded()) PanelMesh->SetStaticMesh(CubeAsset.Object);
	RootComponent = PanelMesh;
	
    TextWidget = CreateDefaultSubobject<UWidgetComponent>(FName("Text Widget"));
	TextWidget->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void ADoorPanel::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADoorPanel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADoorPanel::SetMainText_Implementation(DoorPanelStates Text, DoorPanelTextColours Colour) {}
void ADoorPanel::SetSubText_Implementation(DoorPanelStates Text, DoorPanelTextColours Colour) {}

