// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Actor.h"
#include "L1_Laser.generated.h"

UCLASS()
class PUZZLEMP_API AL1_Laser : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AL1_Laser();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category=Default)
	UStaticMeshComponent* BeamMesh;
	
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category=Default)
	UPointLightComponent* BeamLight;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category=Default)
	UCapsuleComponent* Trigger;

	UPROPERTY(EditInstanceOnly, Category=Default)
	UArrowComponent* Arrow;

	UPROPERTY(EditAnywhere, Category=Default)
	float LaunchStrength = 1000;

	UPROPERTY(EditInstanceOnly, Category=Default)
	bool DoesKill = false;
	
	UFUNCTION()
	void OnTrigger(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void RespawnAndFadeBack(AMyCharacter* Character);

public:
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Disarm();

};
