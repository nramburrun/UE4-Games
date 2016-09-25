// Fill out your copyright notice in the Description page of Project Settings.

#include "BatteryCollector2.h"
#include "Pickup.h"


// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// All Pickups start active
	bIsActive = true;

	//Create the Static Mesh Component
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	//set as root component so that if we want to add more stuff afterwards, they will all be based on the Pickup Mesh
	RootComponent = PickupMesh;


}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
}


//Returns Active State
bool APickup::IsActive()
{
	return bIsActive;
}
//Changes Active State
void APickup::SetActive(bool NewPickupState)
{
	bIsActive = NewPickupState;

}


void APickup::WasCollected_Implementation()
{
	// Log Debug Message
	FString PickupDebugString = GetName();
	UE_LOG(LogClass, Log, TEXT("You have collected %s"), *PickupDebugString);


}