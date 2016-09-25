// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"
#include "BatteryPickup.generated.h"

/**
 * 
 */
UCLASS()
class BATTERYCOLLECTOR2_API ABatteryPickup : public APickup
{
	GENERATED_BODY()
	
public:

	// Sets default values for this actor's properties
	ABatteryPickup();
	
	// Override WasCollected function - use implementaiton because its a blueprint native event

	void WasCollected_Implementation() override;

	// Accessor Function so that public can access the battery's power level
	float GetPower();
protected:
	//set the amount of power the battery gives to player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power", Meta =(BlueprintProtected = "true"))
	float BatteryPower;
	
};
