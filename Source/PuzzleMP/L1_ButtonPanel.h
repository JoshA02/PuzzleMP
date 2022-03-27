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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Reliable)
	void SetButtonState(bool On, int ButtonIndex);

	UFUNCTION()
	void OnButtonPressed(AActor* TriggeringActor, AActor* TriggeredActor);
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category=Default)
	UStaticMeshComponent* ButtonPanel;

	TArray<ATrigger*> Triggers;
	
private:
	UMaterialInstanceDynamic* ButtonMaterial;
	
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* Button1Location;
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* Button2Location;
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* Button3Location;
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* Button4Location;

	USceneComponent* ButtonLocations[4];

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
