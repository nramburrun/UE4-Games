// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BatteryCollector2.h"
#include "BatteryCollector2GameMode.h"
#include "BatteryCollector2Character.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "SpawnVolume.h"

ABatteryCollector2GameMode::ABatteryCollector2GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	//base decay rate
	DecayRate = 0.01f;
}

void ABatteryCollector2GameMode::BeginPlay()
{
	Super::BeginPlay();

	//find all spawn volume actors
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundActors);

	for (auto Actor : FoundActors)
	{
		ASpawnVolume* SpawnVolumeActor = Cast<ASpawnVolume>(Actor);

		if (SpawnVolumeActor)
		{
			SpawnVolumeActors.AddUnique(SpawnVolumeActor);
		}
	}


	SetCurrentState(EBatteryPlayState::EPlaying);
	//set the score to beat
	ABatteryCollector2Character* MyCharacter = Cast<ABatteryCollector2Character>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (MyCharacter)
	{
		PowerToWin = (MyCharacter->GetInitialPower()) * 1.25f;
	}

	if (HUDWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}

}



void ABatteryCollector2GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Check if we are using battery collector character
	ABatteryCollector2Character* MyCharacter = Cast<ABatteryCollector2Character>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (MyCharacter)
	{
		//if power greater than needed to win
		if (MyCharacter->GetCurrentPower() > PowerToWin)
		{
			SetCurrentState(EBatteryPlayState::EWon);
		}
		//character's power is positive
		else if (MyCharacter->GetCurrentPower() > 0)
		{
			//Decrease the power using decay rate
			MyCharacter->UpdatePower(-DeltaTime*DecayRate*(MyCharacter->GetInitialPower()));
		}
		else
		{
			SetCurrentState(EBatteryPlayState::EGameOver);
		}
	}
}

float ABatteryCollector2GameMode::GetPowerToWin() const
{
	return PowerToWin;
}

EBatteryPlayState ABatteryCollector2GameMode::GetCurrentState() const
{
	return CurrentState;
}

void ABatteryCollector2GameMode::SetCurrentState(EBatteryPlayState NewState)
{
	CurrentState = NewState;
	HandleNewState(CurrentState);
}

void ABatteryCollector2GameMode::HandleNewState(EBatteryPlayState NewState)
{
	switch (NewState)
	{
		case EBatteryPlayState::EPlaying:
		{
			//SpawnVolumes to be active
			for (ASpawnVolume* Volume : SpawnVolumeActors)
			{
				Volume->SetSpawningActive(true);
			}
		}
		break;

		case EBatteryPlayState::EWon:
		{
			//SpawmVolumes inactive
			for (ASpawnVolume* Volume : SpawnVolumeActors)
			{
				Volume->SetSpawningActive(false);
			}

		}
		break;

		case EBatteryPlayState::EGameOver:
		{
			//SpawmVolumes inactive'
			for (ASpawnVolume* Volume : SpawnVolumeActors)
			{
				Volume->SetSpawningActive(false);
			}
			//Block player input
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
			if (PlayerController)
			{
				PlayerController->SetCinematicMode(true, false, false, true, true);
			}
			// ragdoll character
			ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this,0);
			
			if (MyCharacter)
			{
				MyCharacter->GetMesh()->SetSimulatePhysics(true);
				MyCharacter->GetMovementComponent()->MovementState.bCanJump = false;
			}
		}
		break;

		default:
		case EBatteryPlayState::EUnknown:
		{
			//donothing
		}
		break;
	}
}