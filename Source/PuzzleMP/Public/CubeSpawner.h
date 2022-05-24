// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Cube.h"
#include "InteractInterface.h"
#include "GameFramework/Actor.h"
#include "CubeSpawner.generated.h"

UCLASS()
class PUZZLEMP_API ACubeSpawner : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubeSpawner();

	UStaticMeshComponent* Mesh;

	UPROPERTY(EditInstanceOnly, Category=Default)
	CubeEnum CubeType = CubeEnum::CUBE_DEFAULT;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, Category=Default)
	int MaxCubes = 1;

	UPROPERTY(EditInstanceOnly, Category=Default)
	float CubeSpawnRate = 5;

	UPROPERTY(EditInstanceOnly, Category=Default)
	TSubclassOf<ACube> CubeClass;
	
private:
	void SpawnObject();
	
	TArray<ACube*> Cubes;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=Interaction)
	void OnInteract(AActor* Caller);

};
