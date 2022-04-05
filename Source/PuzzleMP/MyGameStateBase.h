// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEMP_API AMyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
