// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class BATTERYCOLLECTOR2_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Return the mesh for the pickup
	FORCEINLINE class UStaticMeshComponent* GetMesh() const 
	{ 
		return PickupMesh; 
	}
	// Use Pure for easy/quick functions
	//returns whether picup is active
	UFUNCTION(BlueprintPure, Category = "Pickup")
		bool IsActive();

	//callable only run when the the input exe pin is hit. This is where you put more complex calculations
	//allow ca
	UFUNCTION(BlueprintCallable, Category = "Pickup")
		void SetActive(bool NewPickupState);

	//Function to call when pickup is collected
	UFUNCTION(BlueprintNativeEvent)
		void WasCollected();
		virtual void WasCollected_Implementation();


protected:

	//Variable to tell whether the pickup is active
	// True when the pickup can be used; false when is deactivated
	bool bIsActive;

	

private:
	// Statis MEsh to represent the pickup in the level
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PickupMesh;


	
};
