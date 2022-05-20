// Fill out your copyright notice in the Description page of Project Settings.


#include "L1_ButtonPanel.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AL1_ButtonPanel::AL1_ButtonPanel()
{
	bReplicates = true;

	ButtonPanel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Button Panel Mesh"));
	RootComponent = ButtonPanel;

	Cable = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cable"));
	Cable->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CableAsset(TEXT("/Game/Meshes/L1/SM_L1_CablePanelLaser.SM_L1_CablePanelLaser"));
	if(CableAsset.Succeeded()) Cable->SetStaticMesh(CableAsset.Object);

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

	static ConstructorHelpers::FObjectFinder<USoundAttenuation> ButtonPressSoundAtt (TEXT("/Game/Audio/ButtonAttenuation.ButtonAttenuation"));
	if(ButtonPressSoundAtt.Succeeded()) ButtonPressSoundAttenuation = ButtonPressSoundAtt.Object;
	static ConstructorHelpers::FObjectFinder<USoundBase> ButtonPressSoundAsset(TEXT("/Game/Audio/Wavs/sfx_3p_buttonpress.sfx_3p_buttonpress"));
	if(ButtonPressSoundAsset.Succeeded()) PressSound = ButtonPressSoundAsset.Object;
}

void AL1_ButtonPanel::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME( AL1_ButtonPanel, ButtonStates ); // Sets up replication for ButtonStates
}

void AL1_ButtonPanel::BeginPlay()
{
	Super::BeginPlay();

	ButtonMaterial = ButtonPanel->CreateDynamicMaterialInstance(1);
	CableMaterial = Cable->CreateDynamicMaterialInstance(0);
	
	if(!HasAuthority()) return; // No need for clients to execute anything further

	// Finds the other button panel and stores reference of it to a variable
	TArray<AActor*> Result;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), StaticClass(), Result);
	if(Result.Num() > 2)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("ButtonPanel: Too many button panels in level | Should only be 2 present"), true, true, FColor::Red, 2);
		return;
	}
	for( AActor* Actor : Result )
	{
		if( Cast<AL1_ButtonPanel>(Actor) && Actor != this )
		{
			OtherButtonPanel = Cast<AL1_ButtonPanel>(Actor);
			break;
		}
	}
	if( !OtherButtonPanel || !Laser )
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("ButtonPanel: Could not find other button panel or laser"), true, true, FColor::Red, 2);
		return;
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	for( int x = 0; x < 4; x++ )
	{
		FVector SpawnLocation = ButtonLocations[x]->GetComponentLocation();
		ATrigger* Trigger = GetWorld()->SpawnActor<ATrigger>(SpawnLocation, FRotator(0), SpawnParams);
		Trigger->Enabled = false;
		Trigger->OnTriggerDelegate.AddDynamic(this, &AL1_ButtonPanel::OnButtonPressed);
		Triggers.Add(Trigger);
	}

	StartPuzzle();
}

void AL1_ButtonPanel::StartPuzzle()
{
	if(!HasAuthority()) return;

	// Turn all lights off
	for( int x = 0; x < ButtonStates.Num(); x++ ) SetButtonState(x, false);

	// Temporarily disable all button triggers
	for(ATrigger* Trigger : Triggers) Trigger->Enabled = false;

	// Randomly decide the starting button
	if(RoomIndex == 0) StartButtonIndex = GetRandomButtonIndex();

	// Reset the amount of times the intro loop has played
	IntroLightLoopCount = 0;
	
	// Start the intro loop
	GetWorldTimerManager().SetTimer(IntroLights, this, &AL1_ButtonPanel::FlashLights, 0.6f, true);
}

void AL1_ButtonPanel::FlashLights()
{
	if(!HasAuthority()) return;
	PlayButtonSoundMulticast(0.9);
	if(IntroLightLoopCount >= (IntroLightLoops - 1) * 2 )
	{
		GetWorldTimerManager().ClearTimer(IntroLights);
		if( StartButtonIndex != INDEX_NONE ) SetButtonState(StartButtonIndex, true);
		for( ATrigger* Trigger : Triggers ) Trigger->Enabled = true; // Re-enable button triggers
		return;
	}
	
	const bool CurrentState = ButtonStates[0];

	// Turn on/off each button
	for( int x = 0; x < ButtonStates.Num(); x++ ) SetButtonState(x, !CurrentState);

	// Count how many times the loop's been performed
	IntroLightLoopCount ++;
}

int AL1_ButtonPanel::GetRandomButtonIndex() const
{
	if(!HasAuthority()) return INDEX_NONE; //Ensures only the server is executing this code.

	TArray<int> ButtonIndexes;
	for(int x = 0; x < ButtonStates.Num(); x++)
	{
		if(ButtonStates[x] == false) ButtonIndexes.Add(x);
	}
	if(ButtonIndexes.Num() <= 0)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("TurnOnRandomButton: All buttons already on"), true, true, FColor::Red, 2);
		return INDEX_NONE;
	}

	return ButtonIndexes[FMath::RandRange(0, ButtonIndexes.Num() - 1)];
}


void AL1_ButtonPanel::OnButtonPressed(AActor* TriggeringActor, AActor* TriggeredActor)
{
	if(!HasAuthority()) return;
	
	const int ButtonIndex = Triggers.IndexOfByKey(TriggeredActor);
	const bool ButtonLit = ButtonStates[ButtonIndex];
	
	if(ButtonIndex == INDEX_NONE)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("OnButtonPressed: Pressed button is unregistered"), true, true, FColor::Red, 2);
		return;
	}
	
	//Pressed a button that is already activated. Do nothing.
	if(ButtonLit)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("OnButtonPressed: Pressed a lit button"), true, false, FColor::Blue, 2);
		//TODO: Possibly play a dull tone to indicate that nothing happened, but to still give button feedback to the player.
		PlayButtonSoundMulticast(0.2);
		return;
	}
	if(!ButtonLit)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("OnButtonPressed: Pressed an unlit button"), true, false, FColor::Blue, 2);
	}

	if(OtherButtonPanel->LastLitButtonIndex == ButtonIndex)
	{
		const int OthersLastLit = OtherButtonPanel->LastLitButtonIndex;
		OtherButtonPanel->LastLitButtonIndex = INDEX_NONE;
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("OnButtonPressed: Pressed other player's last lit button"), true, false, FColor::Blue, 2);
		SetButtonState(OthersLastLit, true);

		if(!ButtonStates.Contains(false))
		{
			UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Puzzle complete"), true, true, FColor::Blue, 2);

			PlayButtonSoundMulticast(1.4);

			Laser->Disarm();
			OtherButtonPanel->Laser->Disarm();

			for(ATrigger* Trigger : Triggers) Trigger->Enabled = false;
			for(ATrigger* Trigger : OtherButtonPanel->Triggers) Trigger->Enabled = false;

			TurnOnCable();
			OtherButtonPanel->TurnOnCable();
			
			return;
		}
		PlayButtonSoundMulticast(1.2);
		const int RandomIndex = GetRandomButtonIndex();
		SetButtonState(RandomIndex, true); //Turns on a random light on this panel
		LastLitButtonIndex = RandomIndex;
	}else
	{
		//TODO: Play dull tone to indicate mistake.
		StartPuzzle();
		OtherButtonPanel->StartPuzzle();
		return;
	}
}

void AL1_ButtonPanel::TurnOnCable_Implementation()
{
	CableMaterial->SetScalarParameterValue(FName("State"), 1);
}


void AL1_ButtonPanel::SetButtonState(const int ButtonIndex, const bool NewState)
{
	if(!HasAuthority()) return; // Only allow server to change button states
	ButtonStates[ButtonIndex] = NewState;
	if(NewState) LastLitButtonIndex = ButtonIndex;
	ReflectStateChange();
}

void AL1_ButtonPanel::OnButtonStatesChanged() { ReflectStateChange(); }

void AL1_ButtonPanel::ReflectStateChange()
{
	// Catches times where this gets executed before beginPlay
	if(!ButtonMaterial) ButtonMaterial = ButtonPanel->CreateDynamicMaterialInstance(1);

	// Updates the emissive value of the material
	for(int x = 0; x < ButtonStates.Num(); x++)
	{
		const int ButtonNumber = x + 1;

		FString ParamName = "B";
		ParamName.Append(FString::FromInt(ButtonNumber)).Append("Intensity");

		// Sets param on material (B1Intensity, B2Intensity, etc)
		if( ButtonStates[x] ) ButtonMaterial->SetScalarParameterValue(FName(ParamName), 80);
		else ButtonMaterial->SetScalarParameterValue(FName(ParamName), 0.1);
	}

	
}

void AL1_ButtonPanel::PlayButtonSoundMulticast_Implementation(float Pitch)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), PressSound, GetActorLocation(), FRotator(0), 1, Pitch, 0, ButtonPressSoundAttenuation);
}
