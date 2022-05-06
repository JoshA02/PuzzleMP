// Copyright 2022-2022 Josh Villyat. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "L2_Platform.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRetract);

UCLASS()
class PUZZLEMP_API AL2_Platform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AL2_Platform();

	int GetPlatformNumber();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	float TargetAlpha = 0.0f;
	float Alpha = TargetAlpha;
	UPROPERTY(Replicated)
	bool bDoneMoving = true;
	UPROPERTY(Replicated)
	int MoveState = 0;

	UPROPERTY(EditInstanceOnly)
	int PlatformNumber = 0;

	UPROPERTY(EditInstanceOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditInstanceOnly)
	USceneComponent* ClosedLocation;

	UPROPERTY(EditInstanceOnly)
	UStaticMeshComponent* NumberPanel;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void ExtendPlatform();
	void RetractPlatform();

	bool IsDoneMoving();
	int GetMoveState();

	FOnRetract OnRetractDelegate;
};
