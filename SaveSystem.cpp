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

void USaveSystem::CreateSaveFile(AActor* Actor, FString path)
{
	FString FilePath = UKismetSystemLibrary::GetProjectSavedDirectory() + "save/" + path + ".dat";
	FActorSpawnInfo ActorData;

	APlayerControls* player = Cast<APlayerControls>(Actor);
	
	//---Savings---
	for (int i = 0; i < player->inventory.Num(); i++) //Saves inventory
	{
		ActorData.items[i] = player->inventory[i];
	}
	ActorData.inventoryWeight = player->characterProfile->currentInventoryWeight;

	ActorData.characterHealth = player->characterProfile->characterCurrentHealth;
	ActorData.ActorTransform = Actor->GetTransform();
	//ActorData.ptr = Actor->GetClass();


	//---Savings End---
	//****************************
	FMemoryWriter ActorWriter = FMemoryWriter(ActorData.ActorSaveData, true);
	FSaveArchive Ar(ActorWriter, true);

	Ar << ActorData;

	Actor->Serialize(Ar);

	Ar.FlushCache();
	Ar.Close();

	if (FFileHelper::SaveArrayToFile(ActorData.ActorSaveData, *FilePath)) {}
}

void USaveSystem::LoadSaveFile(AActor* Actor, FString path)
{
	FString FilePath = UKismetSystemLibrary::GetProjectSavedDirectory() + "save/" + path + ".dat";
	if(FPaths::FileExists(FilePath))
	{
		TArray<uint8> BinaryArray;

		if (!FFileHelper::LoadFileToArray(BinaryArray, *FilePath)) return;

		if (BinaryArray.Num() <= 0) return;

		FMemoryReader FromBinary(BinaryArray, true);
		FromBinary.Seek(0);

		FSaveArchive Ar(FromBinary, true);

		FActorSpawnInfo SpawnInfo;
		Ar << SpawnInfo;
		AActor* ActorOut = Actor;

		////Load character location
		ActorOut->SetActorTransform(SpawnInfo.ActorTransform);
		ActorOut->Serialize(Ar);

		////---Loading---

		APlayerControls* player = Cast<APlayerControls>(Actor);

		if (player->characterProfile)
		{
			//SpawnInfo.characterHealth
			player->characterProfile->characterCurrentHealth = 50;
			UE_LOG(LogTemp, Warning, TEXT("%d"), player->characterProfile->characterCurrentHealth);
		}
		UE_LOG(LogTemp, Warning, TEXT("3")); //Burasý çok hýzlý çalýþýyor zamanlayýcý ile yavaþlatmayý dene

		////Load Inventory
		//for (FItemProperties item : SpawnInfo.items)
		//{
		//	if (item.name != "")
		//	{
		//		if (item.texturePath != "")
		//		{
		//			item.texture = LoadObject<UTexture>(nullptr, *item.texturePath);
		//		}
		//		if (item.skeletalMeshPath != "")
		//		{
		//			item.skeletalMesh = LoadObject<USkeletalMesh>(nullptr, *item.skeletalMeshPath);
		//		}

		//		player->inventory.Add(item);
		//	}
		//}
		//player->characterProfile->currentInventoryWeight = SpawnInfo.inventoryWeight;

		////---Loading Done---
		////***********************
		FromBinary.FlushCache();
		BinaryArray.Empty();
		FromBinary.Close();
	}
}

void USaveSystem::OnLevelLoad()
{
	for (AActor* actor : level->GetLoadedLevel()->Actors)
	{
		if (actor->GetClass()->GetSuperClass()->GetName() == "BP_NPC_Management_C")
		{
			//UE_LOG(LogTemp, Warning, TEXT("%s"), *actor->GetName());
			//APlayerControls* npc = Cast<APlayerControls>(actor);

			LoadSaveFile(actor, actor->GetName());
		}
	}

	//UGameplayStatics::UnloadStreamLevel(GetWorld(), FName("SaveLevel"), FLatentActionInfo(), true);
}

void USaveSystem::SaveGame(AActor* Actors, FString path)
{



	CreateSaveFile(Actors, path);
}

void USaveSystem::LoadGame(AActor* Actor, FString path)
{
	

	level = UGameplayStatics::GetStreamingLevel(GetWorld(), FName("SaveLevel"));
	level->OnLevelLoaded.AddDynamic(this, &USaveSystem::OnLevelLoad);

	UGameplayStatics::LoadStreamLevel(GetWorld(), FName("SaveLevel"), true, true, FLatentActionInfo());



	//LoadSaveFile(Actor, path);
}

