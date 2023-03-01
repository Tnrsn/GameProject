// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFinderComp.h"
#include "../PlayerControls.h"
#include "NPC_Management.h"

UEnemyFinderComp::UEnemyFinderComp()
{

}

AActor* UEnemyFinderComp::PickEnemy()
{
	if (nearbyActors.Num() > 0)
	{
		//...
	}

	return nullptr;
}
