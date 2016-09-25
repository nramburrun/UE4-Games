// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class BATTERYCOLLECTOR2_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//Returns the whereToSpawn Subobject - public accessor
	FORCEINLINE class UBoxComponent* GetWhereToSpawn() const 
	{
		return WhereToSpawn;
	}
	 
	//Find a random point within the BoxComponent
	UFUNCTION(BlueprintPure, Category ="Spawning" )
	FVector GetRandomPointInVolume();

	//Toggles whether or not spawn volume spawns pickups
	UFUNCTION(BlueprintCallable, Category="Spawning")
	void SetSpawningActive(bool bShouldSpawn);
protected:
	//pickup where to spawn
	//i guess subclass allows all the things from Apickup to generate whatospawn
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class APickup> WhatToSpawn;

	FTimerHandle SpawnTimer;

	// Minimum Spawn Delay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="Spawning")
	float SpawnDelayRangeLow;

	// Maximum Spawn Delay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float SpawnDelayRangeHigh;

private:
	// Box Component to specify where pickups should be spawned
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category ="Spawning", meta =(AllowPrivateAccess = "true"))
	class UBoxComponent* WhereToSpawn;
	


	// Handle spawning a new pickup
	void SpawnPickup();

	// The Current Spawn Delay
	float SpawnDelay;
};
