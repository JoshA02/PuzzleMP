// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Cube.generated.h"

UCLASS()
class PUZZLEMP_API ACube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACube();

	UPROPERTY(BlueprintReadWrite)
	UStaticMeshComponent* CubeMesh;

	UPROPERTY()
	UBoxComponent* Trigger;

	void Destroy();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UMaterialInstanceDynamic* CubeMaterial;
	
	bool BeingDestroyed = false;
	bool Destroyed = false;
	
	float CurrentCubeState = 1;
	bool CubeState = true; // true = Visible | false = Destroyed
	float StateChangeSpeed = 2.25; // 1/StateChangeSpeed = The duration taken to change states (visible/hidden)
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
