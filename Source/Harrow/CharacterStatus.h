// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/ActorComponent.h"
#include "CharacterStatus.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HARROW_API UCharacterStatus : public UActorComponent
{
	GENERATED_BODY()

// Public structs
public:	
	// Sets default values for this component's properties
	UCharacterStatus();

// Protected functions
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

// Public functions
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Called at an interval, used to modify the stamina pool
	UFUNCTION(BlueprintCallable)
	void StaminaUpdate();

	UFUNCTION(BlueprintCallable)
	void AddDamage(float damageAmount, float poiseDamage);

	UFUNCTION(BlueprintCallable)
	void AddDamageWithForce(float damageAmount, float poiseDamage, FVector pushDirection, float pushForce);

// Private functions
private:
	// Turn on stamina regeneration
	void TriggerStaminaRegen();

	// Used to start flinching the character
	void StartFlinching();

	// Used to stop flinching the character
	void StopFlinching();

// Public variables
public:
	// The current health of the character
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHealth;
	
	// The maximum health of the character
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth;

	// The current stamina of the character
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentStamina;

	// The maximum stamina of the character
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxStamina;

	// List containing all current costs to stamina
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> StaminaList;
	
	// The amount of stamina regenerated every stamina update interval
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StaminaRegenRate;

	// The current poise damage the player has
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentPoise;

	// The maximum poise the player can recieve before they flinch
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxPoise;

	// Boolean for if the character is flinching
	bool Flinching = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool dodging;

// Private variables
private:
	// The timer hanlding the delay on stamina regeneration
	FTimerHandle _regenDelayTimer;

	// Boolean used to make sure the timer only fires once
	bool _regenTimerCheck = false;

	// If stamina can be regenerated
	bool _canStaminaRegen;

	// Timer handler used to delay the stop of flinching
	FTimerHandle _flinchTriggerTimer;

	// If the character can flinch, so the character can't flinch more than once at a time
	bool _canFlinch = true;
};
