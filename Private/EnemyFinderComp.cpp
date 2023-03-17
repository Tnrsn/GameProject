// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFinderComp.h"
#include "../PlayerControls.h"
#include "NPC_Management.h"

UEnemyFinderComp::UEnemyFinderComp()
{

}

AActor* UEnemyFinderComp::PickEnemy()
{
	if (nearbyEnemies.Num() > 0)
	{
		for (AActor* actor : nearbyEnemies)
		{
			APlayerControls* enemy = Cast<APlayerControls>(actor);

			if (!pickedActor)
			{
				pickedActor = actor;
			}
			else if (pickedActor)
			{
				APlayerControls* pickedChar = Cast<APlayerControls>(pickedActor);
				if (enemy->characterProfile->characterCurrentHealth < pickedChar->characterProfile->characterCurrentHealth ||
					pickedChar->characterProfile->characterCurrentHealth <= 0) //If previous picked enemy died It also switches to new enemy
				{
					pickedActor = enemy;
				}
			}
		}

		return pickedActor;
	}
	else
	{
		pickedActor = nullptr;
		return nullptr;
	}
}
