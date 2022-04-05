// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "CubeButton.generated.h"

UCLASS()
class PUZZLEMP_API ACubeButton : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubeButton();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditInstanceOnly, Category=Default)
	AActor* PoweredActor;

	UPROPERTY()
	UBoxComponent* TriggerBox;

	UPROPERTY()
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditInstanceOnly, Category=Default)
	TSubclassOf<AActor> RequiredPresserType;

	UFUNCTION()
	void TriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void TriggerStop(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
