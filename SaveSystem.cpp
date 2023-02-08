// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveSystem.h"

#include <AIController.h>
//#include <GameFramework/Controller.h>
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

	ActorData.currentWorldName = player->currentWorldName;
	ActorData.controlledCharIndex = player->controlledCharIndex;

	ActorData.charIndex = player->charIndex;
	ActorData.inGroup = player->inGroup;

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
	if(FPaths::FileExists(FilePath))
	{
		TArray<uint8> BinaryArray;

		if (!FFileHelper::LoadFileToArray(BinaryArray, *FilePath)) return false;
		if (BinaryArray.Num() <= 0) return false;

		//UE_LOG(LogTemp, Warning, TEXT("%s: %s"), *Actor->GetName(), *path);
		FMemoryReader FromBinary(BinaryArray, true);
		FromBinary.Seek(0);

		FSaveArchive Ar(FromBinary, true);

		FActorSpawnInfo SpawnInfo;
		Ar << SpawnInfo;
		AActor* ActorOut = Actor;

		//Load character location

		//---Loading---

		APlayerControls* player = Cast<APlayerControls>(Actor);

		if (SpawnInfo.currentWorldName != GetWorld()->GetName() && player->GetClass()->GetSuperClass()->GetName() == FString("PlayerControls"))
		{
			UGameplayStatics::OpenLevel(GetWorld(), *SpawnInfo.currentWorldName);
			player->newLevelLoaded = true;
			return false;
		}

		ActorOut->SetActorTransform(SpawnInfo.ActorTransform);
		ActorOut->Serialize(Ar);

		if (!player->characterProfile) player->DispatchBeginPlay();

		player->currentWorldName = SpawnInfo.currentWorldName;
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

		player->charIndex = SpawnInfo.charIndex;
		player->inGroup = SpawnInfo.inGroup;

		//---Loading Done---
		//***********************
		FromBinary.FlushCache();
		BinaryArray.Empty();
		FromBinary.Close();
		return true;
	}
	return false;
}

void USaveSystem::OnLevelLoad()
{
	APlayerControls* playerSave = Cast<APlayerControls>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	//playerSave->ControlNPC(0); //Create a new control npc for player switching that wont crash
	playerSave = playerSave->groupMembers[0];
	SwitchToMainCharacter(playerSave);
	if (!LoadSaveFile(playerSave, playerSave->GetName().Left(playerSave->GetName().Len() - 4)))
	{
		return;
	}
	

	TArray<AActor*> persistentLevelActors = GetWorld()->PersistentLevel->Actors;

	for (AActor* actor : level->GetLoadedLevel()->Actors)
	{
		if (actor->GetClass()->GetSuperClass()->GetName() == "BP_NPC_Management_C")
		{
			FString actorName = actor->GetName().Left(actor->GetName().Len() - 4);
			AActor* newActor = GetWorld()->SpawnActor<ANPC_Management>(actor->GetClass(), FVector::ZeroVector, FRotator::ZeroRotator);

			if (LoadSaveFile(newActor, actorName))
			{
				ANPC_Management* NPCSave = Cast<ANPC_Management>(newActor);

				FVector Location;
				FRotator Rotation;
				FActorSpawnParameters Params;
				TSubclassOf<AAIController> AIController = AAIController::StaticClass();

				AAIController* NPCAI = GetWorld()->SpawnActor<AAIController>(AIController, Location, Rotation, Params);
				NPCAI->Possess(NPCSave);

				if (NPCSave->inGroup)
				{
					LoadGroupMembers(playerSave, NPCSave);
				}

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

	UGameplayStatics::UnloadStreamLevel(GetWorld(), FName("SaveLevel"), FLatentActionInfo(), true);
}

void USaveSystem::SaveGame()
{
	TArray<AActor*> persistentLevelActors = GetWorld()->PersistentLevel->Actors;

	for (AActor* actor : persistentLevelActors)
	{
		if (actor && 
			(actor->GetClass()->GetSuperClass()->GetName() == FString("BP_NPC_Management_C") 
				|| actor->GetClass()->GetSuperClass()->GetName() == FString("PlayerControls")))
		{
			CreateSaveFile(actor, actor->GetName().Left(actor->GetName().Len() - 4));
		}
	}
}

void USaveSystem::LoadGame()
{
	level = UGameplayStatics::GetStreamingLevel(GetWorld(), FName("SaveLevel"));
	level->OnLevelLoaded.AddDynamic(this, &USaveSystem::OnLevelLoad);
	UGameplayStatics::LoadStreamLevel(GetWorld(), FName("SaveLevel"), true, true, FLatentActionInfo());
}

void USaveSystem::LoadGroupMembers(APlayerControls* playerSave, APlayerControls* NPCSave)
{
	if (NPCSave->charIndex != 0)
	{
		if (playerSave->groupMembers.Num() < NPCSave->charIndex + 1)
		{
			playerSave->groupMembers.SetNum(NPCSave->charIndex + 1);
			playerSave->groupMembers[NPCSave->charIndex] = NPCSave;
			NPCSave->groupMembers = playerSave->groupMembers;
		}
		else
		{
			playerSave->groupMembers[NPCSave->charIndex] = NPCSave;
			NPCSave->groupMembers = playerSave->groupMembers;
		}
	}
	else
	{
		if (playerSave->groupMembers.Num() < playerSave->charIndex + 1)
		{
			playerSave->groupMembers.SetNum(playerSave->charIndex + 1);
			playerSave->groupMembers[playerSave->charIndex] = NPCSave;
			NPCSave->groupMembers = playerSave->groupMembers;
		}
		else
		{
			playerSave->groupMembers[playerSave->charIndex] = NPCSave;
			NPCSave->groupMembers = playerSave->groupMembers;
		}

		NPCSave->charIndex = playerSave->charIndex;
		playerSave->charIndex = 0;
	}

	NPCSave->controlledChar = playerSave;
}

void USaveSystem::SwitchToMainCharacter(APlayerControls* player)
{
	if (player->charIndex != 0)
	{
		player->groupMembers[player->charIndex]->GetController()->Possess(player);
	}

	int groupSize = player->groupMembers.Num();

	player->groupMembers.SetNum(1);
	player->groupMembers.SetNum(groupSize);

	player->mainHUD->RemoveFromParent();
	player->mainHUD->AddToViewport();

	player->controlledChar = player;
}
