// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	UPROPERTY(EditAnywhere, SaveGame)
		FItemProperties items[1000];
	UPROPERTY(EditAnywhere, SaveGame)
		int inventoryWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		int characterHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		FTransform ActorTransform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		FString currentLevelName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		int controlledCharIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		int charIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		bool inGroup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		TSoftClassPtr<class AActor> ptr = nullptr;

	TArray<uint8> ActorSaveData;

	friend FSaveArchive& operator << (FSaveArchive& Ar, FActorSpawnInfo& ActorData)
	{
		for (int i = 0; i < 1000; i++) //Saves inventory
		{
			Ar << ActorData.items[i].armorBonus;
			Ar << ActorData.items[i].Category;
			Ar << ActorData.items[i].ConsumableEffect;
			Ar << ActorData.items[i].currentAmount;
			Ar << ActorData.items[i].description;
			Ar << ActorData.items[i].effectStrength;
			Ar << ActorData.items[i].effectTime;
			Ar << ActorData.items[i].hideHeadMesh;
			Ar << ActorData.items[i].inInventory;
			Ar << ActorData.items[i].isEquipped;
			Ar << ActorData.items[i].isStackable;
			Ar << ActorData.items[i].damageBonus;
			Ar << ActorData.items[i].maximumAmount;
			Ar << ActorData.items[i].name;
			Ar << ActorData.items[i].rarity;

			//Save skeletal mesh
			if (ActorData.items[i].skeletalMesh != nullptr)
			{
				ActorData.items[i].skeletalMeshPath = ActorData.items->skeletalMesh->GetPathName();
				Ar << ActorData.items[i].skeletalMeshPath;
			}
			else
			{
				ActorData.items[i].skeletalMeshPath = "";
				Ar << ActorData.items[i].skeletalMeshPath;
			}

			//Save Texture
			if (ActorData.items[i].skeletalMesh != nullptr)
			{
				ActorData.items[i].texturePath = ActorData.items->texture->GetPathName();
				Ar << ActorData.items[i].texturePath;
			}
			else
			{
				ActorData.items[i].texturePath = "";
				Ar << ActorData.items[i].texturePath;
			}

			Ar << ActorData.items[i].weapon2Item;
			Ar << ActorData.items[i].WearableType;
			Ar << ActorData.items[i].weight;
		}
		Ar << ActorData.inventoryWeight;

		Ar << ActorData.characterHealth;
		Ar << ActorData.ActorTransform;
		Ar << ActorData.currentLevelName;


		Ar << ActorData.charIndex;
		Ar << ActorData.inGroup;

		Ar << ActorData.ptr;
		Ar << ActorData.ActorSaveData;
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

	UFUNCTION()
		void CreateSaveFile(AActor* Actors, FString path);
	UFUNCTION()
		bool LoadSaveFile(AActor* Actor, FString path);
	UFUNCTION()
		void OnLevelLoad();
	UFUNCTION()
		void SaveGame();
	UFUNCTION()
		void LoadGame();
	UFUNCTION()
		void LoadGroupMembers(APlayerControls* playerSave, APlayerControls* NPCSave);
	UFUNCTION()
		void SwitchToMainCharacter(APlayerControls* player);
};
