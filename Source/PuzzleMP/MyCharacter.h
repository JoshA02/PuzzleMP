// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/VoiceConfig.h"
#include "MyCharacter.generated.h"

UCLASS()
class PUZZLEMP_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	UVOIPTalker* VOIPRef;

	USoundBase* FootstepSound;
	FVector LastFootstepLocation;
	USoundAttenuation* FootstepSoundAtt;
	TSubclassOf<UMatineeCameraShake> WalkShake;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	AActor* GetHeldItem();

	UPROPERTY(EditDefaultsOnly, Category=Pickup)
	USceneComponent* PickupLocation;
	
private:
	const int HoldDistance = 180;
	
	void MoveLeftRight(float value);
	void MoveForwardBack(float value);
	void LookLeftRight(float value);
	void LookUpDown(float value);

	UFUNCTION(Server, Reliable)
	void Interact(FVector CameraForwardVector);
	UFUNCTION(Server, Reliable)
	void StopInteract();

	void InteractInit();
	void StopInteractInit();

	UPROPERTY(Replicated)
	AActor* HeldItem;
	UPROPERTY(Replicated)
	FVector HeldItemPosition;

	UPROPERTY(Replicated)
	FVector CachedForwardVec;
	UFUNCTION(Server, Reliable)
	void Server_UpdateCachedForwardVec(FVector ForwardVector);
	
	UPROPERTY(EditAnywhere, Category = "Camera")
		UCameraComponent* PlayerCamera;
};
