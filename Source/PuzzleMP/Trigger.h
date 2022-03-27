// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Trigger.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTriggerDelegate, AActor*, TriggeringActor, AActor*, TriggerActor);

UCLASS()
class PUZZLEMP_API ATrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrigger();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(BlueprintReadWrite, Category=Default)
	UBoxComponent* Trigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Default)
	FVector TriggerExtent = FVector(10, 10, 10);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category=Default)
	void SetTriggerExtent(FVector NewExtent);

	void OnTrigger(AActor* TriggeringActor);

	UPROPERTY(BlueprintAssignable, Category=Delegates)
	FOnTriggerDelegate OnTriggerDelegate;
};
