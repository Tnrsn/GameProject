// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "EnemyFinderComp.generated.h"

/**
 * 
 */
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEnemyOverlapSignature, UPrimitiveComponent*, OverlappedComp, AActor*, OtherActor);

UCLASS()
class GAMEPROJECT_API UEnemyFinderComp : public USphereComponent
{
	GENERATED_BODY()

public:
	UEnemyFinderComp();

	TArray<AActor*> nearbyActors;
};
