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
			APlayerControls* pickedChar = Cast<APlayerControls>(pickedActor);

			if (!pickedActor)
			{
				pickedActor = actor;
			}
			else if (enemy->characterProfile->characterCurrentHealth < pickedChar->characterProfile->characterCurrentHealth)
			{
				pickedActor = enemy;
			}
		}

		return pickedActor;
	}
	else
	{
		return nullptr;
	}
}
