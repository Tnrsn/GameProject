// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterProfiles.h"
#include <Kismet/GameplayStatics.h>

void UCharacterProfiles::InitRefilling(UWorld* world, float deltaSeconds)
{
	if (world)
	{
		isRefilling = true;
		//FTimerHandle refillTimerHandle;

		world->GetTimerManager().SetTimer(refillTimerHandle, [&, deltaSeconds, world]() {
			if (!dead)
			{
				if (characterMaximumEnergy > characterCurrentEnergy)
				{
					float energyToAdd = (characterMaximumEnergy - characterCurrentEnergy) * 0.000000001f;
					characterCurrentEnergy += energyToAdd;

					// Interpolate the energy value for smoother refills
					characterCurrentEnergy = FMath::FInterpTo(characterCurrentEnergy, characterMaximumEnergy, deltaSeconds, 0.2f);

					if (characterCurrentEnergy + 1 >= characterMaximumEnergy)
					{
						characterCurrentEnergy = characterMaximumEnergy;
					}
				}

				if (characterMaximumHealth > characterCurrentHealth)
				{
					float healthToAdd = (characterMaximumHealth - characterCurrentHealth) * 0.000000001f;
					characterCurrentHealth += healthToAdd;

					// Interpolate the health value for smoother refills
					characterCurrentHealth = FMath::FInterpTo(characterCurrentHealth, characterMaximumHealth, deltaSeconds, 0.2f);

					if (characterCurrentHealth + 1 >= characterMaximumHealth)
					{
						characterCurrentHealth = characterMaximumHealth;
					}
				}

				// Check if both energy and health are at their maximum
				if (characterCurrentEnergy >= characterMaximumEnergy && characterCurrentHealth >= characterMaximumHealth)
				{
					isRefilling = false;
					world->GetTimerManager().ClearTimer(refillTimerHandle);
				}
			}
			else
			{
				characterCurrentEnergy = 0;
				characterCurrentHealth = 0;

				isRefilling = false;
				world->GetTimerManager().ClearTimer(refillTimerHandle);
			}
			}, deltaSeconds, true);
	}
}

void UCharacterProfiles::ChangeStat(int value, TEnumAsByte<FStatTypes> types)
{
	if (statPoints > 0 || (value < 0 && statPoints <= 0))
	{
		if (types == Str && (characterStats.strength > 4 || (characterStats.strength == 4 && value > 0)))
		{
			characterStats.strength += value;
			statPoints -= value;
		}
		else if (types == Dex && (characterStats.dexterity > 4 || (characterStats.dexterity == 4 && value > 0)))
		{
			characterStats.dexterity += value;
			statPoints -= value;
		}
		else if (types == Con && (characterStats.constitution > 4 || (characterStats.constitution == 4 && value > 0)))
		{
			characterStats.constitution += value;
			statPoints -= value;
		}
		else if (types == Int && (characterStats.intelligence > 4 || (characterStats.intelligence == 4 && value > 0)))
		{
			characterStats.intelligence += value;
			statPoints -= value;
		}
		else if (types == Wis && (characterStats.wisdom > 4 || (characterStats.wisdom == 4 && value > 0)))
		{
			characterStats.wisdom += value;
			statPoints -= value;
		}
	}
}

void UCharacterProfiles::RefreshStats()
{
	beginningStats = characterStats;

	//Calculates Maximum Inventory Capacity
	maxInventoryCapacity = (beginningStats.strength * 10) + ((characterStats.strength - beginningStats.strength) * 2);
	//Calculates Maximum Health
	characterMaximumHealth = (beginningStats.constitution * 10) + ((characterStats.constitution - beginningStats.constitution) * 2);

	//Calculates Maximum Energy
	characterMaximumEnergy = (beginningStats.wisdom * 10) + ((characterStats.wisdom - beginningStats.wisdom) * 2);

}

void UCharacterProfiles::HoldEnergyAndHealthAtMax()
{
	if (characterCurrentHealth > characterMaximumHealth)
		characterCurrentHealth = characterMaximumHealth;

	if (characterCurrentEnergy > characterMaximumEnergy)
		characterCurrentEnergy = characterMaximumEnergy;
}

void UCharacterProfiles::StartRefillCooldown(UWorld* world)
{
	if (world)
	{
		world->GetTimerManager().ClearTimer(refillTimerHandle);

		if (refillCooldownTimerHandle.IsValid())
		{
			// Timer is already active, reset it
			world->GetTimerManager().ClearTimer(refillCooldownTimerHandle);
		}
		else
		{
			canRefill = false;
		}

		refillCooldownTimerHandle.Invalidate();
		isRefilling = false;

		world->GetTimerManager().SetTimer(refillCooldownTimerHandle, [this]() {
			canRefill = true;
			refillCooldownTimerHandle.Invalidate();
			}, 4.0f, false);
	}
}
