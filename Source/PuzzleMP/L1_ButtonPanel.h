// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "L1_Laser.h"
#include "Trigger.h"
#include "GameFramework/Actor.h"
#include "L1_ButtonPanel.generated.h"

UCLASS()
class PUZZLEMP_API AL1_ButtonPanel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AL1_ButtonPanel();

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	AL1_Laser* Laser;

	//(0 for host, 1 for other player)
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	int RoomIndex = 0;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	UStaticMeshComponent* Cable;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnButtonPressed(AActor* TriggeringActor, AActor* TriggeredActor);
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category=Default)
	UStaticMeshComponent* ButtonPanel;

	AL1_ButtonPanel* OtherButtonPanel;

	TArray<ATrigger*> Triggers;

	int GetRandomButtonIndex() const;

	UFUNCTION(NetMulticast, Reliable)
	void PlayButtonSoundMulticast(float Pitch);

	USoundBase*PressSound;
	USoundAttenuation* ButtonPressSoundAttenuation;
	
private:
	UMaterialInstanceDynamic* ButtonMaterial;

	UPROPERTY(Replicated)
	UMaterialInstanceDynamic* CableMaterial;

	UPROPERTY(ReplicatedUsing=OnButtonStatesChanged)
	TArray<bool> ButtonStates = {false, false, false, false};
	UFUNCTION()
	void OnButtonStatesChanged(); // Executed for non-server clients when ButtonStates is changed
	void SetButtonState(const int ButtonIndex, const bool NewState); // Updates a button's state | Execute only from server
	void ReflectStateChange(); // Reflects the changes to button state in the material
	
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* Button1Location;
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* Button2Location;
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* Button3Location;
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* Button4Location;

	USceneComponent* ButtonLocations[4];

	void StartPuzzle();

	void FlashLights();
	int StartButtonIndex = INDEX_NONE;
	FTimerHandle IntroLights;
	int IntroLightLoopCount = 0;
	const int IntroLightLoops = 5;
	int LastLitButtonIndex = INDEX_NONE; // The index of the most recently lit button

	UFUNCTION(NetMulticast, Reliable)
	void TurnOnCable();
};
