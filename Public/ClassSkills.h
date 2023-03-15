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

	//Evasion skill properties
	UPROPERTY(BlueprintReadOnly)
		bool evasionActivated = false;
	int charDex = 0;

	//Charge skill variables
	FTimerHandle chargeTimer;
	FVector startLocation;

	UFUNCTION()
		bool isDamageToHostile(ACharacter* player); //Checks for area damages
	UFUNCTION()
		bool CanHit(ACharacter* player); //Checks for single damages

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

	//Rogue Skills
	UFUNCTION()
		void DualWield(ACharacter* player);
	UFUNCTION()
		void Evasion(ACharacter* player);
	UFUNCTION()
		void BacksStab(ACharacter* player);

	//Mage Skills
	UFUNCTION()
		void Blitz(ACharacter* player);
	UFUNCTION()
		void SuperNova(ACharacter* player, FVector target);
	UFUNCTION()
		void Blink(ACharacter* player, FVector target);
};
