// Fill out your copyright notice in the Description page of Project Settings.

#include "BatteryCollector2.h"
#include "BatteryPickup.h"


//Set Default values
ABatteryPickup::ABatteryPickup() 
{
	//As soon as Battery Spawn, it will fall from the sky
	GetMesh()->SetSimulatePhysics(true);
	 
	//Base Power Level Of Battery
	BatteryPower = 150.0f;
}


void ABatteryPickup::WasCollected_Implementation()
{
	// Use Base pickup behavior
	Super::WasCollected_Implementation();
	//Destroy
	Destroy();
}

//Report power level of battery
float ABatteryPickup::GetPower()
{
	return BatteryPower;
}