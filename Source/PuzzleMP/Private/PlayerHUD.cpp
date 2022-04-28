// Copyright 2022-2022 Josh Villyat. All Rights Reserved


#include "PlayerHUD.h"

#include "Kismet/GameplayStatics.h"

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	HUDWidget = CreateWidget(this->GetOwningPlayerController(), HUDWidgetClass);
	HUDWidget->AddToViewport(0);
}

void APlayerHUD::FadeToBlack_Implementation() {}
void APlayerHUD::FadeFromBlack_Implementation() {}
