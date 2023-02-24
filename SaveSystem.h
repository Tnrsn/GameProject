// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Public/CharacterProfiles.h"
#include "Public/MasterItem.h"
#include "Subsystems/WorldSubsystem.h"
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>
#include "SaveSystem.generated.h"

/**
 * 
 */
struct FSaveArchive : public FObjectAndNameAsStringProxyArchive
{
	FSaveArchive(FArchive& InInnerArchive, bool bInLoadIfFindFails) : FObjectAndNameAsStringProxyArchive(InInnerArchive, bInLoadIfFindFails)
	{
		ArIsSaveGame = true;
		ArNoDelta = true;
	}
};

USTRUCT(BlueprintType)
struct FActorSpawnInfo
{
	GENERATED_BODY();

	void SaveItem(FSaveArchive& Ar, FItemProperties& item)
	{
		Ar << item.name;
		Ar << item.description;
		Ar << item.weight;
		Ar << item.Category;
		Ar << item.maximumAmount;
		Ar << item.isStackable;
		Ar << item.currentAmount;
		Ar << item.rarity;
		Ar << item.inInventory;

		Ar << item.ConsumableEffect;
		Ar << item.effectStrength;
		Ar << item.effectTime;

		Ar << item.WearableType;
		Ar << item.armorBonus;
		Ar << item.isEquipped;

		Ar << item.hideHeadMesh;
		Ar << item.damageBonus;
		Ar << item.weapon2Item;

		if (item.skeletalMesh != nullptr)
		{
			item.skeletalMeshPath = item.skeletalMesh->GetPathName();
			Ar << item.skeletalMeshPath;
		}
		else
		{
			item.skeletalMeshPath = "";
			Ar << item.skeletalMeshPath;
		}

		//Save Texture
		if (item.texture != nullptr)
		{
			item.texturePath =item.texture->GetPathName();
			Ar << item.texturePath;
		}
		else
		{
			item.texturePath = "";
			Ar << item.texturePath;
		}
	}

	//---------------------------------------------------------------------------------------------
	//Character stats
	UPROPERTY(EditAnywhere, SaveGame)
		FString charName;

	UPROPERTY(EditAnywhere, SaveGame)
		int charGender;
	UPROPERTY(EditAnywhere, SaveGame)
		int charRace;
	UPROPERTY(EditAnywhere, SaveGame)
		int charClass;

	UPROPERTY(EditAnywhere, SaveGame)
		FCharacterStats beginningStats;
	UPROPERTY(EditAnywhere, SaveGame)
		FCharacterStats characterStats;

	UPROPERTY(EditAnywhere, SaveGame)
		int statPoints;
	UPROPERTY(EditAnywhere, SaveGame)
		int relationWithPlayer;

	//Character inventory
	UPROPERTY(EditAnywhere, SaveGame)
		FItemProperties items[1000];
	UPROPERTY(EditAnywhere, SaveGame)
		int currentInventoryWeight;

	//Character status
	UPROPERTY(EditAnywhere, SaveGame)
		float characterCurrentHealth;

	//Armor
	UPROPERTY(EditAnywhere, SaveGame)
		FCharacterWearables characterArmor;
	//UPROPERTY(EditAnywhere, SaveGame)
	//	int armorRating;

	//Character Location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		FTransform ActorTransform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		FString currentWorldName;

	//Character group info
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
	//	int controlledCharIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		int charIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		bool inGroup;
//---------------------------------------------------------------------------------------^^^^ Datas to save ^^^^----------------------------

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
	//	TSoftClassPtr<class AActor> ptr = nullptr;

	TArray<uint8> ActorSaveData;

	friend FSaveArchive& operator << (FSaveArchive& Ar, FActorSpawnInfo& ActorData)
	{
		//Ar << ActorData.ptr;
		Ar << ActorData.ActorSaveData;

		//----Inventory-----
		for (int i = 0; i < 1000; i++) 
		{
			ActorData.SaveItem(Ar, ActorData.items[i]);
		}
		
		Ar << ActorData.currentInventoryWeight;

		//-----Character Stats-----
		Ar << ActorData.charName;

		Ar << ActorData.charGender;
		Ar << ActorData.charRace;
		Ar << ActorData.charClass;

		Ar << ActorData.beginningStats.strength;
		Ar << ActorData.beginningStats.dexterity;
		Ar << ActorData.beginningStats.constitution;
		Ar << ActorData.beginningStats.wisdom;
		Ar << ActorData.beginningStats.intelligence;

		Ar << ActorData.characterStats.strength;
		Ar << ActorData.characterStats.dexterity;
		Ar << ActorData.characterStats.constitution;
		Ar << ActorData.characterStats.wisdom;
		Ar << ActorData.characterStats.intelligence;

		Ar << ActorData.statPoints;
		Ar << ActorData.relationWithPlayer;

		//-----Character Status-----
		Ar << ActorData.characterCurrentHealth;

		Ar << ActorData.ActorTransform;
		Ar << ActorData.currentWorldName;

		//-----Group values------
		Ar << ActorData.charIndex;
		Ar << ActorData.inGroup;
		
		//----Character Armors-----
		ActorData.SaveItem(Ar, ActorData.characterArmor.head);
		ActorData.SaveItem(Ar, ActorData.characterArmor.top);
		ActorData.SaveItem(Ar, ActorData.characterArmor.hand);
		ActorData.SaveItem(Ar, ActorData.characterArmor.foot);
		ActorData.SaveItem(Ar, ActorData.characterArmor.firstRing);
		ActorData.SaveItem(Ar, ActorData.characterArmor.secondRing);
		ActorData.SaveItem(Ar, ActorData.characterArmor.neck);
		ActorData.SaveItem(Ar, ActorData.characterArmor.weapon1);
		ActorData.SaveItem(Ar, ActorData.characterArmor.weapon2);

		
		return Ar;
	}
};
UCLASS()
class GAMEPROJECT_API USaveSystem : public UWorldSubsystem
{
	GENERATED_BODY()

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
public:
	UPROPERTY()
		ULevelStreaming* level;
	UPROPERTY()
		FString SName = "";
	UPROPERTY()
		bool TSave = false;
	UPROPERTY()
		bool saveLevelSet = false;
	UPROPERTY()
		int timesLoaded = 10;

	UFUNCTION()
		void CreateSaveFile(AActor* Actors, FString path, FString saveName = "", bool transportSave = false);
	UFUNCTION()
		bool LoadSaveFile(AActor* Actor, FString path, FString saveName = "", bool transportSave = false);
	UFUNCTION()
		void OnLevelLoad();
	UFUNCTION()
		void SaveGame(FString saveName = "", bool transportSave = false);
	UFUNCTION()
		void LoadGame(FString saveName = "", bool transportSave = false);
	UFUNCTION()
		void LoadGroupMembers(APlayerControls* playerSave, APlayerControls* NPCSave);
	UFUNCTION()
		void SwitchToMainCharacter(APlayerControls* player);
	UFUNCTION()
		void LoadItem(APlayerControls* player, FItemProperties& item, FItemProperties& playerItem, USkeletalMeshComponent* skeletalMesh = nullptr);
};
