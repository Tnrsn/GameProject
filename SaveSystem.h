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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		TArray<FItemProperties> inventoryData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		int32 inventorySize;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
	//	TArray<int> testt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		int characterHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		FTransform ActorTransform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		TSoftClassPtr<class AActor> ptr = nullptr;

	TArray<uint8> ActorSaveData;

	friend FSaveArchive& operator << (FSaveArchive& Ar, FActorSpawnInfo& ActorData)
	{
		ActorData.inventorySize = ActorData.inventoryData.Num();

		if (ActorData.inventoryData.Num() != 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *ActorData.inventoryData[0].name);
			Ar << ActorData.inventoryData[0].name;
		}


		//for (FItemProperties& item : ActorData.inventoryData)
		//{
			//Ar << item.armorBonus;
			//Ar << item.Category;
			//Ar << item.ConsumableEffect;
			//Ar << item.currentAmount;
			//Ar << item.description;
			//Ar << item.effectStrength;
			//Ar << item.effectTime;
			//Ar << item.hideHeadMesh;
			//Ar << item.inInventory;
			//Ar << item.isEquipped;
			//Ar << item.isStackable;
			//Ar << item.magicalDamageBonus;
			//Ar << item.maximumAmount;
			//Ar << item.name;
			//Ar << item.physicalDamageBonus;
			//Ar << item.rarity;
			//Ar << item.skeletalMesh;
			//Ar << item.texture;
			//Ar << item.weapon2Item;
			//Ar << item.WearableType;
			//Ar << item.weight;
		//}

		//Ar << ActorData.inventoryData;

		//for (int i = 0; i < ActorData.inventoryData.Num(); i++)
		//{
		//	Ar << ActorData.inventoryData[i].armorBonus;
		//	Ar << ActorData.inventoryData[i].Category;
		//	Ar << ActorData.inventoryData[i].ConsumableEffect;
		//	Ar << ActorData.inventoryData[i].currentAmount;
		//	Ar << ActorData.inventoryData[i].description;
		//	Ar << ActorData.inventoryData[i].effectStrength;
		//	Ar << ActorData.inventoryData[i].effectTime;
		//	Ar << ActorData.inventoryData[i].hideHeadMesh;
		//	Ar << ActorData.inventoryData[i].inInventory;
		//	Ar << ActorData.inventoryData[i].isEquipped;
		//	Ar << ActorData.inventoryData[i].isStackable;
		//	Ar << ActorData.inventoryData[i].magicalDamageBonus;
		//	Ar << ActorData.inventoryData[i].maximumAmount;
		//	Ar << ActorData.inventoryData[i].name;
		//	Ar << ActorData.inventoryData[i].physicalDamageBonus;
		//	Ar << ActorData.inventoryData[i].rarity;
		//	Ar << ActorData.inventoryData[i].skeletalMesh;
		//	Ar << ActorData.inventoryData[i].texture;
		//	Ar << ActorData.inventoryData[i].weapon2Item;
		//	Ar << ActorData.inventoryData[i].WearableType;
		//	Ar << ActorData.inventoryData[i].weight;
		//}


		//Ar << ActorData.testt;

		//Ar.Serialize(ActorData.testt.GetData(), ActorData.testt.GetAllocatedSize());
		//if (ActorData.testt.Num() != 0)
		//{
		//	Ar << ActorData.testt[0];
		//}

		//Ar.Serialize(ActorData.inventory.GetData(), ActorData.inventory.GetAllocatedSize());

		Ar << ActorData.characterHealth;
		Ar << ActorData.ActorTransform;
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
	UFUNCTION()
		void Save(AActor* Actor);
	UFUNCTION()
		void Load(AActor* Actor);

};
