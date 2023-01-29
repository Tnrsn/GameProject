// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveSystem.h"

#include "PlayerControls.h"
#include <Serialization/Archive.h>
#include <Serialization/BufferArchive.h>
#include <Kismet/KismetSystemLibrary.h>
#include <UObject/SoftObjectPtr.h>
#include <Serialization/NameAsStringIndexProxyArchive.h>
#include <Misc/FileHelper.h>

bool USaveSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	return World && World->IsGameWorld();
}

void USaveSystem::Save(AActor* Actor)
{
	FString FilePath = UKismetSystemLibrary::GetProjectSavedDirectory() + "Player.sav";
	FActorSpawnInfo ActorData;

	APlayerControls* player = Cast<APlayerControls>(Actor);
	
	//Savings
	for (int i = 0; i < player->inventory.Num(); i++) //Saves inventory
	{
		ActorData.items[i] = player->inventory[i];
	}
	ActorData.inventoryWeight = player->characterProfile->currentInventoryWeight;

	ActorData.characterHealth = player->characterProfile->characterCurrentHealth;
	ActorData.ActorTransform = Actor->GetTransform();
	//ActorData.ptr = Actor->GetClass();

	//****************************
	FMemoryWriter ActorWriter = FMemoryWriter(ActorData.ActorSaveData, true);
	FSaveArchive Ar(ActorWriter, true);

	Ar << ActorData;

	Actor->Serialize(Ar);

	if (FFileHelper::SaveArrayToFile(ActorData.ActorSaveData, *FilePath)){}

	Ar.FlushCache();
	Ar.Close();
}

void USaveSystem::Load(AActor* Actor)
{
	FString FilePath = UKismetSystemLibrary::GetProjectSavedDirectory() + "Player.sav";

	TArray<uint8> BinaryArray;

	if (!FFileHelper::LoadFileToArray(BinaryArray, *FilePath)) return;

	if (BinaryArray.Num() <= 0) return;

	FMemoryReader FromBinary(BinaryArray, true);
	FromBinary.Seek(0);

	FSaveArchive Ar(FromBinary, true);

	FActorSpawnInfo SpawnInfo;
	Ar << SpawnInfo;
	AActor* ActorOut = Actor;

	//Load character location
	ActorOut->SetActorTransform(SpawnInfo.ActorTransform);
	ActorOut->Serialize(Ar);

	//Player Datas

	APlayerControls* player = Cast<APlayerControls>(Actor);

	player->characterProfile->characterCurrentHealth = SpawnInfo.characterHealth;

	//Load Inventory
	for (FItemProperties item : SpawnInfo.items)
	{
		if (item.name != "")
		{
			if (item.texturePath != "")
			{
				item.texture = LoadObject<UTexture>(nullptr, *item.texturePath);
			}
			if (item.skeletalMeshPath != "")
			{
				item.skeletalMesh = LoadObject<USkeletalMesh>(nullptr, *item.skeletalMeshPath);
			}

			player->inventory.Add(item);
		}
	}
	player->characterProfile->currentInventoryWeight = SpawnInfo.inventoryWeight;


	//***********************
	FromBinary.FlushCache();
	BinaryArray.Empty();
	FromBinary.Close();

}

//FSaveArchive USaveSystem::SaveCharacters(AActor* Actor)
//{
//	FString FilePath = UKismetSystemLibrary::GetProjectSavedDirectory() + "Player.sav";
//	FActorSpawnInfo ActorData;
//
//	APlayerControls* player = Cast<APlayerControls>(Actor);
//
//	//Savings
//	for (int i = 0; i < player->inventory.Num(); i++) //Saves inventory
//	{
//		ActorData.items[i] = player->inventory[i];
//	}
//	ActorData.inventoryWeight = player->characterProfile->currentInventoryWeight;
//
//	ActorData.characterHealth = player->characterProfile->characterCurrentHealth;
//	ActorData.ActorTransform = Actor->GetTransform();
//	//ActorData.ptr = Actor->GetClass();
//
//	//****************************
//	FMemoryWriter ActorWriter = FMemoryWriter(ActorData.ActorSaveData, true);
//	FSaveArchive Ar(ActorWriter, true);
//	Ar << ActorData;
//
//
//	return Ar;
//}
