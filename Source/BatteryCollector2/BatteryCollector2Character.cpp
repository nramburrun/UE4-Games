// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BatteryCollector2.h"
#include "BatteryCollector2Character.h"
#include "Pickup.h"
#include "BatteryPickup.h"
//////////////////////////////////////////////////////////////////////////
// ABatteryCollector2Character

ABatteryCollector2Character::ABatteryCollector2Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	//Create Collection Sphere
	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->AttachTo(RootComponent);
	CollectionSphere->SetSphereRadius(200.0f);
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)



	// set a base power level for character
	InitialPower = 2000.0f;
	CharacterPower = InitialPower;

	//set the dependence of the speed on the power level
	SpeedFactor = 0.75f;
	BaseSpeed = 10.0f;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABatteryCollector2Character::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("Collect", IE_Pressed, this, &ABatteryCollector2Character::CollectPickups);

	InputComponent->BindAxis("MoveForward", this, &ABatteryCollector2Character::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABatteryCollector2Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &ABatteryCollector2Character::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ABatteryCollector2Character::LookUpAtRate);

	// handle touch devices
	InputComponent->BindTouch(IE_Pressed, this, &ABatteryCollector2Character::TouchStarted);
	InputComponent->BindTouch(IE_Released, this, &ABatteryCollector2Character::TouchStopped);
}


void ABatteryCollector2Character::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void ABatteryCollector2Character::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StopJumping();
	}
}

void ABatteryCollector2Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABatteryCollector2Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ABatteryCollector2Character::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABatteryCollector2Character::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}


void ABatteryCollector2Character::CollectPickups()
{
	//Get all overlapping Actors and store into an array
	TArray<AActor*> CollectedActors;
	CollectionSphere->GetOverlappingActors(CollectedActors);

	// Keep track of collecting power
	float CollectedPower = 0;

	//Go through the Array, for each actor collected,
	for (int32 iCollected = 0; iCollected < CollectedActors.Num(); ++iCollected)
	{
		// Cast the actor to APickup
		APickup* const TestPickup = Cast<APickup>(CollectedActors[iCollected]);
		// If cast is successful and pickup is valid and active
		if (TestPickup && !TestPickup->IsPendingKill() && TestPickup->IsActive())
		{
			// call the pickup's WasCollected function
			TestPickup->WasCollected();
			//Check to see if pickup is also a battery
			ABatteryPickup* const TestBattery = Cast<ABatteryPickup>(TestPickup);

			if (TestBattery)
			{
				// increase the collected power
				CollectedPower += TestBattery->GetPower();
			}
			// Deactivate the pickup
			TestPickup->SetActive(false);
		}
	}
	if (CollectedPower > 0)
	{
		UpdatePower(CollectedPower);
	}
}

//returns starting power
float ABatteryCollector2Character::GetInitialPower() 
{
	return InitialPower;
}


//Returns current power
float ABatteryCollector2Character::GetCurrentPower()
{
	return CharacterPower;
}

//Called Whenever power is increased or decreased
void ABatteryCollector2Character::UpdatePower(float PowerChange)
{
	//first change power
	CharacterPower = CharacterPower + PowerChange;
	//change speed based on power
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed + SpeedFactor*CharacterPower;
	// call visual effect
	PowerChangeEffect();
}