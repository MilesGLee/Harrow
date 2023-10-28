// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Instantiating Class Components
	PlayerSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Arm"));
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));

	//Attach the Spring Arm to the Character's Skeletal Mesh Component
	PlayerSpringArm->SetupAttachment(GetMesh());

	//Attach the Camera to the SpringArmComponent
	PlayerCamera->AttachToComponent(PlayerSpringArm, FAttachmentTransformRules::KeepRelativeTransform);

	//Setting default properties of the SpringArmComp
	PlayerSpringArm->bUsePawnControlRotation = true;
	PlayerSpringArm->bEnableCameraLag = true;
	PlayerSpringArm->TargetArmLength = 300.0f;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Initialize values
	CanDodge = true;
	MoveSpeed = _baseMoveSpeed;
	PlayerActionList.Empty();
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMovement();
	RotateMesh();
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerCharacter::PlayerMovement(float scaleX, float scaleY)
{
	// Horizontal movement
	FRotator controlRotation= GetController()->GetControlRotation();
	FVector directionR = UKismetMathLibrary::GetRightVector(FRotator(0, controlRotation.Yaw, controlRotation.Roll));

	// Vertical movement
	FVector directionF = UKismetMathLibrary::GetForwardVector(FRotator(0, controlRotation.Yaw, 0));

	InputDirection = UKismetMathLibrary::MakeRotFromX(GetActorForwardVector()).RotateVector(FVector(scaleY, scaleX, 0));

	// Stop moving if the player is dodging or flinching
	if (PlayerActionList.Contains("Dodge") || PlayerActionList.Contains("Flinch")) return;

	AddMovementInput(directionF, scaleY, false);
	AddMovementInput(directionR, scaleX, false);
}

void APlayerCharacter::PlayerLook(float scaleX, float scaleY)
{
	// Horizontal look
	AddControllerYawInput((scaleX * HorizontalLookSensitivity));

	// Vertical look
	AddControllerPitchInput((scaleY * VerticalLookSensitivity));
}

void APlayerCharacter::DodgeTriggered(float elapsedTime)
{
	// If the player has held down the dodge button for more than half a second...
	if (elapsedTime >= 0.5f) 
	{
		// If the player has enough stamina to sprint...
		if (MyStatus->CurrentStamina > 0.0f) 
		{
			// Start making the player sprint
			StartSprinting();
		}
		else 
		{
			StopSprinting();
		}
	}
}

bool APlayerCharacter::DodgeCompleted(float elapsedTime)
{
	// Stop sprinting when the dodge button is released
	StopSprinting();

	// Stop dodging if the player is flinching
	if (PlayerActionList.Contains("Flinch")) return false;

	// If the dodge button was held for less than half a second and the player has enough stamina to dodge...
	if (elapsedTime < 0.5f && MyStatus->CurrentStamina >= 15.0f && CanDodge)
	{
		// Return true for blueprint to call the dodge functionality
		return true;
	}
	else 
	{
		return false;
	}
}

void APlayerCharacter::StartSprinting()
{
	// If the player is flinching, stop sprinting
	if (PlayerActionList.Contains("Flinch")) 
	{
		RemoveFromStaminaList("Sprint");
		return;
	}

	// If the player wants to sprint, and they are not already sprinting...
	if (!_isSprinting)
	{
		// Set players move speed accordingly and add sprinting to the stamina list
		_isSprinting = true;

		MoveSpeed = _baseMoveSpeed * SprintModifier;

		MyStatus->StaminaList.Add("Sprint");
	}
}

void APlayerCharacter::StopSprinting()
{
	// If the player is sprinting...
	if (_isSprinting)
	{
		// Set up to stop sprinting after a delay
		_isSprinting = false;

		GetWorld()->GetTimerManager().SetTimer(_sprintDelayTimer, this, &APlayerCharacter::StopSprintingDelay, 0.3f, false);
	}
}

void APlayerCharacter::StopSprintingDelay()
{
	// Stop sprinting for the player

	MoveSpeed = _baseMoveSpeed;

	if (MyStatus->StaminaList.IsEmpty()) return;

	// Remove all instances of sprinting from the stamina list
	RemoveFromStaminaList("Sprint");
}

void APlayerCharacter::UpdateMovement()
{
	// Set the character controllers move speed to this scripts move speed
	GetComponentByClass<UCharacterMovementComponent>()->MaxWalkSpeed = MoveSpeed;

	// If the player is flinching, from the status component...
	if (MyStatus->Flinching && !_flinchTrigger)
	{
		// Add flinching to the action list and slow player down
		_flinchTrigger = true;
		PlayerActionList.Add("Flinch");
		MoveSpeed = 100.0f;
		
	}
	else if (!MyStatus->Flinching && _flinchTrigger) 
	{
		// Restore default move speed to player
		_flinchTrigger = false;
		MoveSpeed = 600.0f;
		if (!PlayerActionList.IsEmpty()) 
		{
			// Remove all instances of flinching from the action list
			RemoveFromActionList("Flinch");
		}
	}
}

void APlayerCharacter::RemoveFromStaminaList(FString value)
{
	// For loop through the entire list and remove all elements that match the given value
	for (int32 Index = 0; Index != MyStatus->StaminaList.Num(); ++Index)
	{
		if (MyStatus->StaminaList[Index] == value)
		{
			MyStatus->StaminaList.RemoveAt(Index);
		}
		// Make sure we don't get a null return error
		if (MyStatus->StaminaList.IsEmpty()) return;
	}
}

void APlayerCharacter::RemoveFromActionList(FString value)
{
	// For loop through the entire list and remove all elements that match the given value
	for (int32 Index = 0; Index != PlayerActionList.Num(); ++Index)
	{
		if (PlayerActionList[Index] == value)
		{
			PlayerActionList.RemoveAt(Index);
		}
		// Make sure we don't get a null return error
		if (PlayerActionList.IsEmpty()) return;
	}
}

void APlayerCharacter::RotateMesh()
{
	// Always rotate the mesh to the desired rotation over time
	//GetMesh()->SetWorldRotation(FMath::Lerp(GetMesh()->GetComponentRotation(), _desiredMeshRotation, 0.1f));

	if (GetVelocity().Length() != 0.0f)
		GetMesh()->SetWorldRotation(FMath::Lerp(GetMesh()->GetComponentRotation(), _desiredMeshRotation, 0.1f));
	else
		GetMesh()->SetWorldRotation(_desiredMeshRotation);

	// If the player is moving...
	if (GetVelocity().Length() > 0.0f)
	{
		// Find relative rotations and set the desired mesh rotation to them
		FVector desiredForward = PlayerCamera->GetForwardVector() * MoveY;
		FVector desiredRight = PlayerCamera->GetRightVector() * MoveX;
		FRotator desiredRot = UKismetMathLibrary::MakeRotFromX((desiredForward + desiredRight));

		if (desiredRot == FRotator(0, 0, 0)) return;

		desiredRot.Yaw -= 90.0f;
		desiredRot.Pitch = 0.0f;
		desiredRot.Roll = 0.0f;

		_desiredMeshRotation = desiredRot;
	}
}


