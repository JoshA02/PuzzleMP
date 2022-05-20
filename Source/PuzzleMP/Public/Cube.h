// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CubeEnum.h"
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

	void ChangeCubeColour(FVector NewColour);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UMaterialInstanceDynamic* CubeMaterial;

	UPROPERTY(Replicated)
	bool BeingDestroyed = false;
	UPROPERTY(Replicated)
	bool Destroyed = false;

	UPROPERTY(ReplicatedUsing=OnCurrentStateChange)
	float CurrentState = 1;
	UPROPERTY(Replicated)
	bool State = true; // true = Visible | false = Destroyed
	const float StateChangeSpeed = 2.25; // 1/StateChangeSpeed = The duration taken to change states (visible/hidden)
	
	UFUNCTION()
	void OnCurrentStateChange() const;
	void UpdateCurrentState(const float NewState);
	void ReflectStateChange() const;

	UFUNCTION()
	void OnCubeColourChange();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	USoundBase* HitSound;
	FVector LastNI = FVector(0, 0, 0);
	USoundAttenuation* HitSoundAttAsset;
	
	UPROPERTY(ReplicatedUsing=OnCubeColourChange)
	FVector CubeColour = FVector(1);
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool IsBeingDestroyed();
};
