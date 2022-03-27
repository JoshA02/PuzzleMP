// Fill out your copyright notice in the Description page of Project Settings.


#include "L1_ButtonPanel.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("BeginPlay: Laser actor not provided"), true, true, FColor::Red, 2);
		return;
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	for(int x = 0; x < 4; x++)
	{
		FVector Location = ButtonLocations[x]->GetComponentLocation();
		ATrigger* Trigger = GetWorld()->SpawnActor<ATrigger>(Location, FRotator(0, 0, 0), SpawnParams);
		Trigger->SetTriggerExtent(FVector(6,6,6));
		Trigger->OnTriggerDelegate.AddDynamic(this, &AL1_ButtonPanel::OnButtonPressed);
		Triggers.Add(Trigger);
	}

	//Setup dynamic material
	ButtonMaterial = ButtonPanel->CreateDynamicMaterialInstance(1);
	SetButtonState(false, 0);
	SetButtonState(false, 1);
	SetButtonState(false, 2);
	SetButtonState(false, 3);
	
	// if(RoomIndex == 0)
	// {
	// 	//Start of button sequence. TODO: Light up single colour
	// 	SetButtonState(true, FMath::RandRange(0, 3));
	// }else
	// {
	// 	//Start of button sequence. TODO: Light up 0 colours
	// }
}

void AL1_ButtonPanel::OnButtonPressed(AActor* TriggeringActor, AActor* TriggeredActor)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("OnButtonPressed: Called"), true, true, FColor::Blue, 2);
	const int ButtonIndex = Triggers.IndexOfByKey(TriggeredActor);
	if(ButtonIndex == INDEX_NONE)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("OnButtonPressed: No button"), true, true, FColor::Red, 2);
		return;
	}
	SetButtonState(true, ButtonIndex);
}

void AL1_ButtonPanel::SetButtonState_Implementation(bool On, int ButtonIndex)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("SetButtonState: Setting state for button"), true, true, FColor::Blue, 2);
	if(ButtonMaterial == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Unable to find button material"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("SetButtonState: Changing state"));
	
	ButtonIndex ++;
	if(ButtonIndex <= 0 || ButtonIndex > 4)
	{
		UE_LOG(LogTemp, Error, TEXT("Attempted to adjust state of button outside range (0-3)"));
		return;
	}
	FString ParamName = "B";
	ParamName.Append(FString::FromInt(ButtonIndex)).Append("Intensity");
	if(On) ButtonMaterial->SetScalarParameterValue(FName(ParamName), 80);
	else ButtonMaterial->SetScalarParameterValue(FName(ParamName), 0.1);
}

// Called every frame
void AL1_ButtonPanel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

