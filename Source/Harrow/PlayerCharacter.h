// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterStatus.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class HARROW_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

// Public structs
public:
	// Sets default values for this character's properties
	APlayerCharacter();

// Protected functions
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

// Public functions
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Moves the player in the inputted direction
	UFUNCTION(BlueprintCallable)
	void PlayerMovement(float scaleX, float scaleY);

	// Moves the camera based on user input
	UFUNCTION(BlueprintCallable)
	void PlayerLook(float scaleX, float scaleY);

	// Continuously called when the dodge button is pressed
	UFUNCTION(BlueprintCallable)
	void DodgeTriggered(float elapsedTime);

	// Called once when the dodge button is released
	UFUNCTION(BlueprintCallable)
	bool DodgeCompleted(float elapsedTime);

// Private functions
private:
	// Begins making the player sprint
	void StartSprinting();

	// Stops the player from sprinting
	void StopSprinting();

	// Second stop function with a delay, stops the player from sprinting
	void StopSprintingDelay();

	// Updates the players movement based on the action list
	void UpdateMovement();

	// Removes all instances of the given value from the Character Status's Stamina List
	void RemoveFromStaminaList(FString value);

	// Removes all instances of the given value from the Action List
	void RemoveFromActionList(FString value);

	// Rotate the players mesh independantly of the character itself
	void RotateMesh();

// Public variables
public:
	// The players camera
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UCameraComponent* PlayerCamera;

	// The spring arm the player camera is attached to
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USpringArmComponent* PlayerSpringArm;

	// The players movement in the x direction
	UPROPERTY(BlueprintReadWrite)
	float MoveX;

	// The players movement in the y direction
	UPROPERTY(BlueprintReadWrite)
	float MoveY;

	// How fast the player currently moves
	UPROPERTY(BlueprintReadWrite)
	float MoveSpeed = 0.75f;

	// Sensitivity multiplier for camera controls
	UPROPERTY(BlueprintReadWrite)
	float HorizontalLookSensitivity = 1.00f;

	// Sensitivity multiplier for camera controls
	UPROPERTY(BlueprintReadWrite)
	float VerticalLookSensitivity = 0.50f;

	// Combined vector giving the input direction
	UPROPERTY(BlueprintReadWrite)
	FVector InputDirection;

	// The players character status component
	UPROPERTY(BlueprintReadWrite)
	UCharacterStatus* MyStatus;

	// If the player is able to dodge
	UPROPERTY(BlueprintReadWrite)
	bool CanDodge;

	// Multiplier for increasing the move speed while sprinting
	UPROPERTY(EditAnywhere)
	float SprintModifier;

	// The action list which holds most actions the player can commit
	TArray<FString> PlayerActionList;

// Private variables
private:
	// If the player is currently sprinting
	bool _isSprinting;

	// Timer that handles the delay on stopping from sprinting
	FTimerHandle _sprintDelayTimer;

	// The players default move speed
	float _baseMoveSpeed = 600.00f;

	// Trigger to make sure we dont have more than one flinch
	bool _flinchTrigger = false;

	// The desired rotation that the mesh should be using
	FRotator _desiredMeshRotation;
};

