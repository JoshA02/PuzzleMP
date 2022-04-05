// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractInterface.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class PUZZLEMP_API ADoor : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

	UPROPERTY(BlueprintReadWrite, Category=Default)
	UStaticMeshComponent* LeftDoor;
	UPROPERTY(BlueprintReadWrite, Category=Default)
	UStaticMeshComponent* RightDoor;

	UMaterialInstanceDynamic* DoorMaterial;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	float State = 0.0f;
	UPROPERTY(ReplicatedUsing=OnCurrentStateChange)
	float CurrentState = 0.0f;

	UFUNCTION()
	void OnCurrentStateChange() const;
	void UpdateCurrentState(const float NewState);
	void ReflectStateChange() const;

private:
	FVector InitialLocation;
	const float StateChangeSpeed = 2.25; // 1/StateChangeSpeed = The duration taken to change states (visible/hidden)
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=Interaction)
	void OnInteract(AActor* Caller);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=Interaction)
	void OnStopInteract(AActor* Caller);
	
	void OpenDoors();
	void CloseDoors();
};
