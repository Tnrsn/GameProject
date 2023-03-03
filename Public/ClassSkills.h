// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterProfiles.h"
#include "UObject/NoExportTypes.h"
#include "ClassSkills.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UClassSkills : public UObject
{
	GENERATED_BODY()
	

public:
	UFUNCTION()
		void SkillOne(TEnumAsByte<FCharacterClasses> charClass);
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
		void Charge();
};
