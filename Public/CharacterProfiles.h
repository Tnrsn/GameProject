// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MasterItem.h"
#include "UObject/NoExportTypes.h"
#include "CharacterProfiles.generated.h"

/**
 * 
 */
UENUM(BlueprintType, Category = "Character Properties")
enum FPlayerClasses
{
	Warrior,
	Mage,
	Rogue
};
USTRUCT(BlueprintType, Category = "Character Properties")
struct FCharacterStats
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Properties")
		TEnumAsByte<FPlayerClasses> playerClass;
	UPROPERTY(Blueprintreadonly, EditAnywhere, Category = "Character Properties")
		int strength = 0;
	UPROPERTY(Blueprintreadonly, EditAnywhere, Category = "Character Properties")
		int dexterity = 0;
	UPROPERTY(Blueprintreadonly, EditAnywhere, Category = "Character Properties")
		int constitution = 0;
	UPROPERTY(Blueprintreadonly, EditAnywhere, Category = "Character Properties")
		int intelligence = 0;
	UPROPERTY(Blueprintreadonly, EditAnywhere, Category = "Character Properties")
		int wisdom = 0;
};
USTRUCT(BlueprintType, Category = "Character Properties")
struct FCharacterWearables
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadOnly)
		FItemProperties head;
	UPROPERTY(BlueprintReadOnly)
		FItemProperties top;
	UPROPERTY(BlueprintReadOnly)
		FItemProperties hand;
	UPROPERTY(BlueprintReadOnly)
		FItemProperties foot;
	UPROPERTY(BlueprintReadOnly)
		FItemProperties firstRing;
	UPROPERTY(BlueprintReadOnly)
		FItemProperties secondRing;
	UPROPERTY(BlueprintReadOnly)
		FItemProperties neck;
	UPROPERTY(BlueprintReadOnly)
		FItemProperties weapon1;
	UPROPERTY(BlueprintReadOnly)
		FItemProperties weapon2;
};
UCLASS()
class GAMEPROJECT_API UCharacterProfiles : public UObject
{
	GENERATED_BODY()
	

public:
	UPROPERTY() //It'll be between -100 and 100
		int relationWithPlayer = 0;

	UPROPERTY(BlueprintReadOnly)
		float characterMaximumHealth;
	UPROPERTY(BlueprintReadOnly)
		float characterCurrentHealth;

	UPROPERTY(BlueprintReadOnly)
		int armorRating = 0;

	UPROPERTY(BlueprintReadOnly)
		int maxInventoryCapacity = 0;
	UPROPERTY(BlueprintReadOnly)
		int currentInventoryWeight = 0;

	UPROPERTY()
		FCharacterStats beginningStats;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Stats")
		FCharacterStats characterStats;
	UPROPERTY(BlueprintReadOnly)
		FCharacterWearables characterArmor;
};
