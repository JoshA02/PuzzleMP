// Fill out your copyright notice in the Description page of Project Settings.


#include "L1_ButtonPanel.h"

#include <string>

#include "MyPlayerController.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AL1_ButtonPanel::AL1_ButtonPanel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	ButtonPanel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_ButtonPanel"));
	RootComponent = ButtonPanel;

	Button1Location = CreateDefaultSubobject<USceneComponent>(TEXT("Button 1 Location"));
	Button2Location = CreateDefaultSubobject<USceneComponent>(TEXT("Button 2 Location"));
	Button3Location = CreateDefaultSubobject<USceneComponent>(TEXT("Button 3 Location"));
	Button4Location = CreateDefaultSubobject<USceneComponent>(TEXT("Button 4 Location"));
	Button1Location->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Button2Location->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Button3Location->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Button4Location->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ButtonLocations[0] = Button1Location;
	ButtonLocations[1] = Button2Location;
	ButtonLocations[2] = Button3Location;
	ButtonLocations[3] = Button4Location;
}

// Called when the game starts or when spawned
void AL1_ButtonPanel::BeginPlay()
{
	Super::BeginPlay();
	if(Laser == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Laser actor not provided"))
		return;
	}

	for(int x = 0; x < 4; x++)
	{
		ATrigger* Trigger = GetWorld()->SpawnActor<ATrigger>(ButtonLocations[x]->GetComponentLocation(), FRotator(0,0,0), FActorSpawnParameters());
		Trigger->SetOwner(this);
		Trigger->SetTriggerExtent(FVector(6, 6, 6));
		Trigger->AttachToComponent(ButtonLocations[x], FAttachmentTransformRules::KeepWorldTransform);
		Triggers.Add(Trigger);
		Triggers[x]->OnTriggerDelegate.AddDynamic(this, &AL1_ButtonPanel::OnButtonPressed); //Implement trigger indexes or trigger names. This way, we can know which button is being pressed.
		//TODO: Make sure the visual stuff (changing button state; on/off) is changed to be a multicast function, executed only from the server originally.
		UE_LOG(LogTemp, Log, TEXT("Spawned trigger"));
	}

	//Setup dynamic material
	ButtonMaterial = ButtonPanel->CreateDynamicMaterialInstance(1);
	SetButtonState(false, 0);
	SetButtonState(false, 1);
	SetButtonState(false, 2);
	SetButtonState(false, 3);
	
	if(RoomIndex == 0)
	{
		//Start of button sequence. TODO: Light up single colour
		SetButtonState(true, FMath::RandRange(0, 3));
		
	}else
	{
		//Start of button sequence. TODO: Light up 0 colours
	}
}

void AL1_ButtonPanel::OnButtonPressed(AActor* TriggeringActor)
{
	UE_LOG(LogTemp, Log, TEXT("Button was pressed"));
}

void AL1_ButtonPanel::SetButtonState(bool On, int ButtonIndex)
{
	if(ButtonMaterial == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Unable to find button material"));
		return;
	}

	FName ParamName = "B1Intensity";
	switch(ButtonIndex){
		case 0:
			ParamName = "B1Intensity";
			break;
		case 1:
			ParamName = "B2Intensity";
			break;
		case 2:
			ParamName = "B3Intensity";
			break;
		case 3:
			ParamName = "B4Intensity";
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("Attempted to adjust state of button outside range (0-3)"));
			return;
		
	}
	if(On) ButtonMaterial->SetScalarParameterValue(ParamName, 80);
	else ButtonMaterial->SetScalarParameterValue(ParamName, 0.1);
}

// Called every frame
void AL1_ButtonPanel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

