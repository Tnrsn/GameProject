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
enum FStatTypes
{
	Str,
	Dex,
	Con,
	Int,
	Wis
};
UENUM(BlueprintType, Category = "Character Properties")
enum FCharacterGender
{
	Male,
	Female
};
UENUM(BlueprintType, Category = "Character Properties")
enum FCharacterRace
{
	Human,
	Elf
};
UENUM(BlueprintType, Category = "Character Properties")
enum FCharacterClasses
{
	Warrior,
	Mage,
	Rogue
};
USTRUCT(BlueprintType, Category = "Character Properties")
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(Blueprintreadonly, EditAnywhere, Category = "Character Properties")
		int strength = 8;
	UPROPERTY(Blueprintreadonly, EditAnywhere, Category = "Character Properties")
		int dexterity = 8;
	UPROPERTY(Blueprintreadonly, EditAnywhere, Category = "Character Properties")
		int constitution = 8;
	UPROPERTY(Blueprintreadonly, EditAnywhere, Category = "Character Properties")
		int intelligence = 8;
	UPROPERTY(Blueprintreadonly, EditAnywhere, Category = "Character Properties")
		int wisdom = 8;
};
USTRUCT(BlueprintType, Category = "Character Properties")
struct FCharacterWearables
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FItemProperties head;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FItemProperties top;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FItemProperties hand;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FItemProperties foot;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FItemProperties firstRing;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FItemProperties secondRing;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FItemProperties neck;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FItemProperties weapon1;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FItemProperties weapon2;
};
UCLASS()
class GAMEPROJECT_API UCharacterProfiles : public UObject
{
	GENERATED_BODY()
	

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Name")
		FString charName;

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

	UPROPERTY(BlueprintReadOnly)
		int statPoints = 12;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FCharacterStats beginningStats;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Stats")
		FCharacterStats characterStats;
	UPROPERTY(BlueprintReadOnly)
		FCharacterWearables characterArmor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Properties")
		TEnumAsByte<FCharacterGender> charGender;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Properties")
		TEnumAsByte<FCharacterRace> charRace;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Properties")
		TEnumAsByte<FCharacterClasses> charClass;

	UFUNCTION(BlueprintCallable)
		void ChangeStat(int value, TEnumAsByte<FStatTypes> types);
};
