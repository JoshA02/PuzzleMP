// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	void Disarm();

	
	UPROPERTY(BlueprintReadWrite, Category=Default)
	UStaticMeshComponent* BeamMesh;
	
	UPROPERTY(BlueprintReadWrite, Category=Default)
	UPointLightComponent* BeamLight;

	UPROPERTY(BlueprintReadWrite, Category=Default)
	UCapsuleComponent* Trigger;

	UFUNCTION()
	void OnTrigger(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
