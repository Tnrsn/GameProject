// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MasterItem.h"
#include <Kismet/GameplayStatics.h>
#include "Blueprint/UserWidget.h"
#include "ManageWidgets.generated.h"

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

	UPROPERTY(BlueprintReadOnly, Category = "Character Properties")
		TEnumAsByte<FPlayerClasses> playerClass;
	UPROPERTY(Blueprintreadonly, Category = "Character Properties")
		int strength = 0;
	UPROPERTY(Blueprintreadonly, Category = "Character Properties")
		int dexterity = 0;
	UPROPERTY(Blueprintreadonly, Category = "Character Properties")
		int constitution = 0;
	UPROPERTY(Blueprintreadonly, Category = "Character Properties")
		int intelligence = 0;
	UPROPERTY(Blueprintreadonly, Category = "Character Properties")
		int wisdom = 0;
};
USTRUCT(BlueprintType, Category = "Character Properties")
struct FCharacterArmor
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
};
UCLASS()
class GAMEPROJECT_API UManageWidgets : public UUserWidget
{
	GENERATED_BODY()

public:
	UManageWidgets(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable)
		void ButtonPressed();
	UFUNCTION(BlueprintCallable)
		void ButtonReleased();

	UFUNCTION(BlueprintCallable)
		float PlayerHealthBar();


	//PlayerStats
	float playerMaximumHealth = 100;
	UPROPERTY(BlueprintReadWrite)
	float playerCurrentHealth = playerMaximumHealth;

	UPROPERTY(BlueprintReadOnly)
		int armorRating = 0;

	UPROPERTY(BlueprintReadOnly)
		int maxInventoryCapacity = 0;
	UPROPERTY(BlueprintReadOnly)
		int currentInventoryWeight = 0;

	UPROPERTY()
		FCharacterStats beginningStats;
	UPROPERTY(BlueprintReadOnly)
		FCharacterStats characterStats;
	UPROPERTY(BlueprintReadOnly)
		FCharacterArmor characterArmor;
};
