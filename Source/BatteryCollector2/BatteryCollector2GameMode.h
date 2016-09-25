// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "BatteryCollector2GameMode.generated.h"


//enum to store the current state of gameplay
UENUM(BlueprintType)
enum class EBatteryPlayState
{
	EPlaying,
	EGameOver,
	EWon,
	EUnknown
};

UCLASS(minimalapi)
class ABatteryCollector2GameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABatteryCollector2GameMode();

	virtual void Tick(float DeltaTime) override;

	// Returns power neede to win
	UFUNCTION(BlueprintPure, Category ="Power")
	float GetPowerToWin() const;

	virtual void BeginPlay() override;
	// Returns current Playing state
	UFUNCTION(BlueprintPure, Category = "Power")
	EBatteryPlayState GetCurrentState() const;

	//sets a new playing state
	void SetCurrentState(EBatteryPlayState NewState);
protected:
	// Rate at which character loses power
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category ="Power", Meta = (BlueprintProtect="true"))
	float DecayRate;

	//Power needed to win the game
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category ="Power", Meta = (BlueprintProtect = "true"))
	float PowerToWin;

	// The widget class to use for HUD screen
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtect = "true"))
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	// the instance of hte HUD
	UPROPERTY()
	class UUserWidget* CurrentWidget;



private:
	//Keeps track of the current playing state
	EBatteryPlayState CurrentState;

	TArray<class ASpawnVolume*> SpawnVolumeActors;

	//Handle any function calls that rely upon changint playing state of our game
	void HandleNewState(EBatteryPlayState NewState);


};



