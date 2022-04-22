// Copyright 2022-2022 Josh Villyat. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "DoorPanel.generated.h"

UCLASS()
class PUZZLEMP_API ADoorPanel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorPanel();

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	UWidgetComponent* TextWidget;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
