// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveSystem.h"

#include "Public/NPC_Management.h"
#include <Engine/Level.h>
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

	ActorData.currentLevelName = player->currentLevelName;
	ActorData.controlledCharIndex = player->controlledCharIndex;

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

bool USaveSystem::LoadSaveFile(AActor* Actor, FString path)
{
	FString FilePath = UKismetSystemLibrary::GetProjectSavedDirectory() + "save/" + path + ".dat";
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *path);
	if(FPaths::FileExists(FilePath))
	{
		TArray<uint8> BinaryArray;

		if (!FFileHelper::LoadFileToArray(BinaryArray, *FilePath)) return false;
		if (BinaryArray.Num() <= 0) return false;

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

		if (SpawnInfo.currentLevelName != GetWorld()->GetName()) return false;
		if (!player->characterProfile) player->DispatchBeginPlay();

		player->currentLevelName = SpawnInfo.currentLevelName;
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

				//UE_LOG(LogTemp, Warning, TEXT("%s"), *item.name);
				player->inventory.Add(item);
			}
		}
		player->characterProfile->currentInventoryWeight = SpawnInfo.inventoryWeight;

		//Save group members


		////---Loading Done---
		////***********************
		FromBinary.FlushCache();
		BinaryArray.Empty();
		FromBinary.Close();
		return true;
	}
	return false;
}

void USaveSystem::OnLevelLoad()
{
	TArray<AActor*> persistentLevelActors = GetWorld()->PersistentLevel->Actors;

	for (AActor* actor : level->GetLoadedLevel()->Actors)
	{
		if (actor->GetClass()->GetSuperClass()->GetName() == "BP_NPC_Management_C")
		{
			FString actorName = actor->GetName().Left(actor->GetName().Len() - 4);
			AActor* newActor = GetWorld()->SpawnActor<ANPC_Management>(actor->GetClass(), FVector::ZeroVector, FRotator::ZeroRotator);

			if (LoadSaveFile(newActor, actorName))
			{
				for (AActor* persistentActor : persistentLevelActors)
				{
					if (persistentActor && persistentActor->GetName().Left(persistentActor->GetName().Len() - 4) == actorName)
					{
						persistentActor->Destroy();
						break;
					}
				}
			}
			else
			{
				newActor->Destroy();
			}
		}
	}

	APlayerControls* playerSave = Cast<APlayerControls>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	LoadSaveFile(playerSave->groupMembers[0], playerSave->groupMembers[0]->GetName());
	UGameplayStatics::UnloadStreamLevel(GetWorld(), FName("SaveLevel"), FLatentActionInfo(), true);
}

void USaveSystem::SaveGame(AActor* Actors, FString path)
{



	CreateSaveFile(Actors, path.Left(path.Len() - 4));
}

void USaveSystem::LoadGame()
{
	

	level = UGameplayStatics::GetStreamingLevel(GetWorld(), FName("SaveLevel"));
	level->OnLevelLoaded.AddDynamic(this, &USaveSystem::OnLevelLoad);

	UGameplayStatics::LoadStreamLevel(GetWorld(), FName("SaveLevel"), true, true, FLatentActionInfo());



	//LoadSaveFile(Actor, path);
}


//void USaveSystem::OnLevelLoad()
//{
//	for (AActor* actor : level->GetLoadedLevel()->Actors)
//	{
//		if (actor->GetClass()->GetSuperClass()->GetName() == "BP_NPC_Management_C")
//		{
//			AActor* newActor = GetWorld()->SpawnActor<ANPC_Management>(actor->GetClass(), FVector::ZeroVector, FRotator::ZeroRotator);
//
//			if (LoadSaveFile(newActor, newActor->GetName().Left(newActor->GetName().Len() - 4)))
//			{
//				for (AActor* persistentLevelActor : GetWorld()->PersistentLevel->Actors)
//				{
//					if (persistentLevelActor)
//					{
//						if (persistentLevelActor->GetName().Left(persistentLevelActor->GetName().Len() - 4) == newActor->GetName().Left(newActor->GetName().Len() - 4))
//						{
//							UE_LOG(LogTemp, Warning, TEXT("Destroyed"));
//							persistentLevelActor->Destroy();
//							break;
//						}
//					}
//				}
//			}
//			else
//			{
//				newActor->Destroy();
//			}
//		}
//	}
//
//	APlayerControls* playerSave = Cast<APlayerControls>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
//	LoadSaveFile(playerSave->groupMembers[0], playerSave->groupMembers[0]->GetName());
//	UGameplayStatics::UnloadStreamLevel(GetWorld(), FName("SaveLevel"), FLatentActionInfo(), true);
//}