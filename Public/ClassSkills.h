// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Engine/World.h>
#include "DamageZone.h"
#include "CharacterProfiles.h"
#include "UObject/NoExportTypes.h"
#include <Components/SphereComponent.h>
#include <Components/CapsuleComponent.h>
#include <GameFramework/Character.h>
#include "ClassSkills.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UClassSkills : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
		bool skillOneTargeting = false;
	UPROPERTY(BlueprintReadOnly)
		bool skillTwoTargeting = false;
	UPROPERTY(BlueprintReadOnly)
		bool skillThreeTargeting = false;

	//Charge skill variables
	FTimerHandle chargeTimer;
	FVector startLocation;

	UFUNCTION()
		bool isDamageToHostile(ACharacter* player);

	UFUNCTION()
		void SkillOne(TEnumAsByte<FCharacterClasses> charClass, ACharacter* player, FVector target);
	UFUNCTION()
		void SkillTwo(TEnumAsByte<FCharacterClasses> charClass, ACharacter* player, FVector target);
	UFUNCTION()
		void SkillThree(TEnumAsByte<FCharacterClasses> charClass, ACharacter* player, FVector target);

	//Warrior Skills
	UFUNCTION()
		void Charge(ACharacter* player, FVector target);
	UFUNCTION()
		void WhirlWind(ACharacter* player);
	UFUNCTION()
		void PowerStrike(ACharacter* player);
};
