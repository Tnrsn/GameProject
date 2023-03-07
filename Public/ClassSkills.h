// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Engine/World.h>
#include "CharacterProfiles.h"
#include "UObject/NoExportTypes.h"
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

	//Charge skill variables
	FTimerHandle chargeTimer;
	FVector startLocation;

	UFUNCTION()
		void SkillOne(TEnumAsByte<FCharacterClasses> charClass, ACharacter* player, FVector target);
	UFUNCTION()
		void SkillTwo(TEnumAsByte<FCharacterClasses> charClass);
	UFUNCTION()
		void SkillThree(TEnumAsByte<FCharacterClasses> charClass);
	UFUNCTION()
		void SkillFour(TEnumAsByte<FCharacterClasses> charClass);
	UFUNCTION()
		void SkillFive(TEnumAsByte<FCharacterClasses> charClass);


	//Warrior Skills
	UFUNCTION()
		void Charge(ACharacter* player, FVector target);
};
