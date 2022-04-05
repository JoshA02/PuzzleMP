// Fill out your copyright notice in the Description page of Project Settings.


#include "L1_ButtonPanel.h"

#include "DrawDebugHelpers.h"
#include "MyGameStateBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AL1_ButtonPanel::AL1_ButtonPanel()
{
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

void AL1_ButtonPanel::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME( AL1_ButtonPanel, ButtonStates );
}

void AL1_ButtonPanel::BeginPlay()
{
	Super::BeginPlay();

	ButtonMaterial = ButtonPanel->CreateDynamicMaterialInstance(1);

	//Finds the other button panel and stores reference of it to a variable
	TArray<AActor*> Result;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), StaticClass(), Result);
	for(AActor* Actor : Result)
	{
		if(Cast<AL1_ButtonPanel>(Actor) && Actor != this)
		{
			OtherButtonPanel = Cast<AL1_ButtonPanel>(Actor);
			break;
		}
	}
	if(!OtherButtonPanel)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("BeginPlay: Could not find other button panel"), true, true, FColor::Red, 2);
		return;
	}
	if(Laser == nullptr)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("BeginPlay: Laser actor not provided"), true, true, FColor::Red, 2);
		return;
	}


	//Trigger setup
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	for(int x = 0; x < 4; x++)
	{
		FVector Location = ButtonLocations[x]->GetComponentLocation();
		ATrigger* Trigger = GetWorld()->SpawnActor<ATrigger>(Location, FRotator(0, 0, 0), SpawnParams);
		Trigger->SetTriggerExtent(FVector(6,6,6));
		Trigger->OnTriggerDelegate.AddDynamic(this, &AL1_ButtonPanel::OnButtonPressed);
		Trigger->Enabled = false;
		Triggers.Add(Trigger);
	}

	//Updates button visuals to match their states (all off by default)
	UpdateButtonStates();
	StartPuzzle();
}

void AL1_ButtonPanel::StartPuzzle()
{
	//Make sure the host machine is running this.
	if(!HasAuthority()) return;

	//Turn all lights off.
	for(int x = 0; x < ButtonStates.Num(); x++) SetButtonState(x, false);

	for(ATrigger* Trigger : Triggers) Trigger->Enabled = false;
	FakeLitButton = TPairInitializer<int, bool>{0, false};
	
	//If this is room 0, turn on a random button's light.
	if(RoomIndex == 0) StartButtonIndex = GetRandomButtonIndex();
	
	GetWorldTimerManager().SetTimer(IntroLights, this, &AL1_ButtonPanel::FakeLightButton, 0.1f, true, 2.0f);
}

//Possibly change to have all lights blink on/off simultaneously (IntroLightLoops times).
void AL1_ButtonPanel::FakeLightButton()
{
	for(int x = 0; x < ButtonStates.Num(); x++) SetButtonState(x, false); //Turn all buttons off.

	//If the index is higher than the last button, means it's just finished a loop.
	if(FakeLitButton.Key > ButtonStates.Num() - 1)
	{
		//Check if it's looped IntroLightLoops - 1 times. If it has, stop here.
		if(FakeLitButton.Value == IntroLightLoops - 1)
		{
			GetWorldTimerManager().ClearTimer(IntroLights);
			for(ATrigger* Trigger : OtherButtonPanel->Triggers) Trigger->Enabled = true;
			for(ATrigger* Trigger : Triggers) Trigger->Enabled = true;
			return;
		}

		//Otherwise, continue by resetting the index to 0 and increasing the value (amount of loops)
		FakeLitButton.Key = 0; //Reset to start of sequence.
		FakeLitButton.Value ++; //Increment the loop counter.
	}
	
	SetButtonState(FakeLitButton.Key, true); //Turn this button on.
	if(StartButtonIndex == FakeLitButton.Key && FakeLitButton.Value == IntroLightLoops - 1) //If the randomly picked starting button is the same as the lit one, and it's at the last loop, stop here.
	{
		LastLitButtonIndex = StartButtonIndex;
		GetWorldTimerManager().ClearTimer(IntroLights);
		return;
	}
	FakeLitButton.Key ++;
}

void AL1_ButtonPanel::SetButtonState(int ButtonIndex, bool NewState)
{
	if(!HasAuthority())
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("SetButtonState: Function aborted; no authority"), true, true, FColor::Red, 2);
		return;
	}
	ButtonStates[ButtonIndex] = NewState;
	UpdateButtonStates(); //Updates for server.
}



void AL1_ButtonPanel::OnButtonPressed(AActor* TriggeringActor, AActor* TriggeredActor)
{
	const int ButtonIndex = Triggers.IndexOfByKey(TriggeredActor);
	if(ButtonIndex == INDEX_NONE)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("OnButtonPressed: Pressed button is unregistered"), true, true, FColor::Red, 2);
		return;
	}
	
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("OnButtonPressed: Button Pressed"), false, true, FColor::Blue, 2);
	//Pressed a button that is already activated. Do nothing.
	if(ButtonStates[ButtonIndex])
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("OnButtonPressed: Pressed a lit button"), true, false, FColor::Blue, 2);
		//TODO: Possibly play a dull tone to indicate that nothing happened, but to still give button feedback to the player.
		return;
	}
	if(!ButtonStates[ButtonIndex])
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("OnButtonPressed: Pressed an unlit button"), true, false, FColor::Blue, 2);
	}

	// const TArray<bool> OtherButtonStates = OtherButtonPanel->ButtonStates;
	if(OtherButtonPanel->LastLitButtonIndex == ButtonIndex)
	{
		const int OthersLastLit = OtherButtonPanel->LastLitButtonIndex;
		OtherButtonPanel->LastLitButtonIndex = INDEX_NONE;
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("OnButtonPressed: Pressed other player's last lit button"), true, false, FColor::Blue, 2);
		SetButtonState(OthersLastLit, true);

		if(!ButtonStates.Contains(false))
		{
			Laser->Disarm();
			OtherButtonPanel->Laser->Disarm();
			return;
		}
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

int AL1_ButtonPanel::GetRandomButtonIndex()
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
	// ButtonStates[ButtonIndexes[FMath::RandRange(0, ButtonIndexes.Num() - 1)]] = true;
	UpdateButtonStates(); //Executes it for the server.
}

void AL1_ButtonPanel::OnChangeButtonState()
{
	// UKismetSystemLibrary::PrintString(GetWorld(), TEXT("OnChangeButtonState: Detected change"), false, true, FColor::Blue, 2);
	UpdateButtonStates(); //Executes it for the clients.
}

void AL1_ButtonPanel::UpdateButtonStates()
{
	if(!ButtonMaterial) ButtonMaterial = ButtonPanel->CreateDynamicMaterialInstance(1); //Catches times where this gets executed before beginPlay
	
	for(int x = 0; x < ButtonStates.Num(); x++)
	{
		const int ButtonIndex = x + 1;
		if(ButtonIndex <= 0 || ButtonIndex > 4)
		{
			UE_LOG(LogTemp, Error, TEXT("Attempted to adjust state of button outside range (0-3)"));
			return;
		}
		
		bool On = ButtonStates[x];
		FString ParamName = "B";
		ParamName.Append(FString::FromInt(ButtonIndex)).Append("Intensity");
		if(On) ButtonMaterial->SetScalarParameterValue(FName(ParamName), 80);
		else ButtonMaterial->SetScalarParameterValue(FName(ParamName), 0.1);
	}
}

