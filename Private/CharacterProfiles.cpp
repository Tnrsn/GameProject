// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterProfiles.h"
#include <Kismet/GameplayStatics.h>

void UCharacterProfiles::InitRefilling(UWorld* world)
{
	world->GetTimerManager().SetTimer(energyTimer, FTimerDelegate::CreateLambda([=]() {
	if (!dead)
	{
		
		if (characterMaximumEnergy > characterCurrentEnergy)
		{
			characterCurrentEnergy += 5;
			if (characterCurrentEnergy > characterMaximumEnergy)
				characterCurrentEnergy = characterMaximumEnergy;
		}

		if (characterMaximumHealth > characterCurrentHealth)
		{
			characterCurrentHealth += 5;
			if (characterCurrentHealth > characterMaximumHealth)
				characterCurrentHealth = characterMaximumHealth;
		}
	}
	else
	{
		characterCurrentEnergy = 0;
		characterCurrentHealth = 0;
	}
	}), 1.5f, true);
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

