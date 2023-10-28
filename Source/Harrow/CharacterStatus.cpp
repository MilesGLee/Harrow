// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterStatus.h"

// Sets default values for this component's properties
UCharacterStatus::UCharacterStatus()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCharacterStatus::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	CurrentStamina = MaxStamina;
	CurrentHealth = MaxHealth;
}


// Called every frame
void UCharacterStatus::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCharacterStatus::StaminaUpdate()
{
	// Clamp stamina values
	if (CurrentStamina > (MaxStamina)) CurrentStamina = MaxStamina;
	if (CurrentStamina < 0) CurrentStamina = 0;

	// If the stamina list is unpopulated...
	if (StaminaList.IsEmpty()) 
	{
		if (CurrentStamina >= MaxStamina) return;

		// Call to begin regenerating stamina.
		if(!_canStaminaRegen)
		{
			// Make sure we do not call the timer more than once
			if (!_regenTimerCheck) {
				_regenTimerCheck = true;
				GetWorld()->GetTimerManager().SetTimer(_regenDelayTimer, this, &UCharacterStatus::TriggerStaminaRegen, 2.0f, false);
			}
		}
		else 
		{
			// Regenerate stamina
			CurrentStamina += StaminaRegenRate;
		}
	}
	// If the stamina list is populated...
	else 
	{
		// Stop stamina regen
		_canStaminaRegen = false;

		// Begin subtracting stamina costs
		for (int32 Index = 0; Index != StaminaList.Num(); ++Index)
		{
			// Return function if we have no stamina left to subtract
			if (CurrentStamina <= 0) return;

			if (StaminaList[Index] == "Sprint")
			{
				CurrentStamina -= 1;
			}
			if (StaminaList[Index] == "Dodge")
			{
				CurrentStamina -= 15;
				StaminaList.RemoveAt(Index);
				break;
			}
		}
	}
}

void UCharacterStatus::AddDamage(float damageAmount, float poiseDamage)
{
	if (dodging) return;

	// If the character should die...
	if ((CurrentHealth - damageAmount) <= 0) 
	{
		// ADD DEATH FUNCTIONALITY HERE
		return;
	}
	// Damage the character
	CurrentHealth -= damageAmount;

	// Add poise damage to the character
	CurrentPoise += poiseDamage;

	// If the character has reached their poise limit, flinch them
	if (CurrentPoise >= MaxPoise) 
	{
		if (_canFlinch) 
		{
			StartFlinching();
		}
	}
}

void UCharacterStatus::AddDamageWithForce(float damageAmount, float poiseDamage, FVector pushDirection, float pushForce)
{
	if (dodging) return;

	// If the character should die...
	if ((CurrentHealth - damageAmount) <= 0)
	{
		// ADD DEATH FUNCTIONALITY HERE
		return;
	}
	// Damage the character
	CurrentHealth -= damageAmount;

	// Add poise damage to the character
	CurrentPoise += poiseDamage;

	// If the character has reached their poise limit, flinch them
	if (CurrentPoise >= MaxPoise)
	{
		if (_canFlinch)
		{
			StartFlinching();
		}
	}

	ACharacter* ownerCharacter = Cast<ACharacter>(GetOwner());

	if (ownerCharacter == nullptr) return;

	ownerCharacter->GetComponentByClass<UCharacterMovementComponent>()->AddImpulse((pushDirection * pushForce));
}

void UCharacterStatus::TriggerStaminaRegen()
{
	// After a delay of not consuming stamina, begin regenerate it
	_regenTimerCheck = false;
	if (StaminaList.IsEmpty()) 
	{
		_canStaminaRegen = true;
	}
}

void UCharacterStatus::StartFlinching()
{
	// Begin flinching the character, then stop after a delay
	_canFlinch = false;
	Flinching = true;
	GetWorld()->GetTimerManager().SetTimer(_flinchTriggerTimer, this, &UCharacterStatus::StopFlinching, 0.2f, false);
}

void UCharacterStatus::StopFlinching()
{
	// Stop the character from flinching
	Flinching = false;
	_canFlinch = true;
	CurrentPoise = 0;
}

