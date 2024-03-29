// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Engine/World.h>
#include "DamageZone.h"
#include "DefaultGameInstance.h"
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

private:
	FTimerHandle decalTimer;

public:
	AActor* owner;

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
	FVector currentLocation;
	
	//Decals
	UPROPERTY()
		AAbilityDecal* abilityAreaDecal;
	UPROPERTY()
		AAbilityDecal* selectionDecal;


	UFUNCTION()
		bool isDamageToHostile(ACharacter* player); //Checks for area damages
	UFUNCTION()
		bool CanHit(ACharacter* player, AActor* enemy); //Checks for single damages

	UFUNCTION()
		void SkillOne(TEnumAsByte<FCharacterClasses> charClass, ACharacter* player, FVector target, AActor* enemy = nullptr);
	UFUNCTION()
		void SkillTwo(TEnumAsByte<FCharacterClasses> charClass, ACharacter* player, FVector target, AActor* enemy = nullptr);
	UFUNCTION()
		void SkillThree(TEnumAsByte<FCharacterClasses> charClass, ACharacter* player, FVector target, AActor* enemy = nullptr);

	//Warrior Skills
	UFUNCTION()
		void Charge(ACharacter* player, FVector target);
	UFUNCTION()
		void WhirlWind(ACharacter* player);
	UFUNCTION()
		bool PowerStrike(ACharacter* player, AActor* enemy);

	//Rogue Skills
	UFUNCTION()
		bool DualWield(ACharacter* player, AActor* enemy);
	UFUNCTION()
		void Evasion(ACharacter* player);
	UFUNCTION()
		bool BacksStab(ACharacter* player, AActor* enemy);

	//Mage Skills
	UFUNCTION()
		bool Blitz(ACharacter* player, AActor* enemy);
	UFUNCTION()
		bool SuperNova(ACharacter* player, FVector target);
	UFUNCTION()
		bool Blink(ACharacter* player, FVector target);

	//VFX
	UFUNCTION()
		void PlaySparks(ACharacter* player, FVector location, FVector scale = FVector(1));
	UFUNCTION()
		void HandleAbilityAreaDecal(ACharacter* player, float abilityAreaSize, float selectionAreaSize);

	UFUNCTION()
		void CancelSkillTargetings();
	UFUNCTION()
		void DestroyDecals();
};
