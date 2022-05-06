// Fill out your copyright notice in the Description page of Project Settings.


#include "L1_Laser.h"

#include "Cube.h"
#include "MyCharacter.h"
#include "MyGameStateBase.h"
#include "MyPlayerController.h"
#include "SavingUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AL1_Laser::AL1_Laser()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	BeamMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Beam Mesh"));
	RootComponent = BeamMesh;
	BeamLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Beam Light"));
	BeamLight->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Trigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger"));
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	Trigger->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AL1_Laser::OnTrigger);
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AL1_Laser::BeginPlay() { Super::BeginPlay(); }

//Hides the laser and disables any overlap events - Multicast
void AL1_Laser::Disarm_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Disarm laser"));
	BeamMesh->SetVisibility(false);
	BeamLight->SetVisibility(false);
	Trigger->SetGenerateOverlapEvents(false);
}

void AL1_Laser::OnTrigger(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!HasAuthority()) return;
	AMyCharacter* Character = Cast<AMyCharacter>(OtherActor);
	ACube* Cube = Cast<ACube>(OtherActor);
	if(Character)
	{
		if(!DoesKill)
		{
			Character->LaunchCharacter(Arrow->GetForwardVector() * LaunchStrength, false, false); // If not killing, just launch the player
		}

		if(DoesKill)
		{
			AMyPlayerController* PC = Cast<AMyPlayerController>(Character->GetController());
			if(!PC) return;
			PC->Owner_FadeHUDToBlack(); // Fade to black
			
			FTimerHandle UnusedHandle;
			FTimerDelegate TimerDel;
			TimerDel.BindUObject(this, &AL1_Laser::RespawnAndFadeBack, Character);
			GetWorldTimerManager().SetTimer(UnusedHandle, TimerDel, 2, false);
		}
	}
	if(Cube) Cube->Destroy();
}

// Triggered via a timer, 2 seconds after fading to black.
void AL1_Laser::RespawnAndFadeBack(AMyCharacter* Character)
{
	if(!HasAuthority()) return; // Only execute for server

	const AMyGameStateBase* GameState = Cast<AMyGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	if(!GameState)
	{
		UE_LOG(LogTemp, Error, TEXT("L1_Laser: GameState undefined"));
		return;	
	}
	int PlayerIndex = GameState->PlayerArray.Find(Character->GetPlayerState());
	if(!PlayerIndex)
	{
		UE_LOG(LogTemp, Error, TEXT("L1_Laser: PlayerIndex undefined"));
		PlayerIndex = 0;
	}
	Character->SetActorTransform(SavingUtils::GetSpawnLocations()[PlayerIndex]);
	
	AMyPlayerController* PC = Cast<AMyPlayerController>(Character->GetController());
	if(!PC) return;
	PC->Owner_FadeHUDFromBlack(); // Fade from black
}