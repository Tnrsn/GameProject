// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveSystem.h"

#include <Engine.h>
#include <AIController.h>
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

void USaveSystem::CreateSaveFile(AActor* Actor, FString path, FString saveName, bool transportSave)
{
	FString FilePath;
	if (transportSave)
	{
		FilePath = UKismetSystemLibrary::GetProjectSavedDirectory() + "Saves/" + "TransportSave/" + path + ".dat";
	}
	else
	{
		FilePath = UKismetSystemLibrary::GetProjectSavedDirectory() + "Saves/" + saveName + "/" + path + ".dat";
	}
	FActorSpawnInfo ActorData;

	APlayerControls* player = Cast<APlayerControls>(Actor);
	
	//---------------------------------------------------------------------------------------------Savings-------------------------------------------------
	for (int i = 0; i < player->inventory.Num(); i++) //Saves inventory
	{
		ActorData.items[i] = player->inventory[i];
	}
	ActorData.currentInventoryWeight = player->characterProfile->currentInventoryWeight;

	//Character Location
	ActorData.ActorTransform = Actor->GetTransform();
	ActorData.currentWorldName = player->currentWorldName;

	//ActorData.ptr = Actor->GetClass();

	//Character Stats
	ActorData.charName = player->characterProfile->charName;

	ActorData.charGender = player->characterProfile->charGender.GetValue();
	ActorData.charRace = player->characterProfile->charRace.GetValue();
	ActorData.charClass = player->characterProfile->charClass.GetValue();

	ActorData.beginningStats = player->characterProfile->beginningStats;
	ActorData.characterStats = player->characterProfile->characterStats;

	ActorData.statPoints = player->characterProfile->statPoints;
	ActorData.relationWithPlayer = player->characterProfile->relationWithPlayer;

	//Character Status
	ActorData.characterCurrentHealth = player->characterProfile->characterCurrentHealth;

	//Group info
	//ActorData.controlledCharIndex = player->controlledCharIndex;

	ActorData.charIndex = player->charIndex;
	ActorData.inGroup = player->inGroup;

	//Characte Armor
	ActorData.characterArmor.top = player->characterProfile->characterArmor.top;

	//----------------------------------------------------------------------------------------Savings End---------------------------------------------
	//****************************
	FMemoryWriter ActorWriter = FMemoryWriter(ActorData.ActorSaveData, true);
	FSaveArchive Ar(ActorWriter, true);

	Ar << ActorData;

	Actor->Serialize(Ar);

	Ar.FlushCache();
	Ar.Close();

	if (FFileHelper::SaveArrayToFile(ActorData.ActorSaveData, *FilePath)) {}
}

bool USaveSystem::LoadSaveFile(AActor* Actor, FString path, FString saveName, bool transportSave)
{
	FString FilePath;

	if (transportSave)
	{
		FilePath = UKismetSystemLibrary::GetProjectSavedDirectory() + "Saves/" + "TransportSave/" + path + ".dat";
	}
	else
	{
		FilePath = UKismetSystemLibrary::GetProjectSavedDirectory() + "Saves/" + saveName + "/" + path + ".dat";
	}

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

		//Load character location

		//----------------------------------------------------------------------------------------------------------------Loading--------------------------------------------------

		APlayerControls* player = Cast<APlayerControls>(Actor);
		
		if (transportSave && !SpawnInfo.inGroup) return false;

		//Character Location
		if (!player->newLevelLoaded && !APlayerControls::toNewWorld && SpawnInfo.currentWorldName != GetWorld()->GetName() 
			&& player->GetClass()->GetSuperClass()->GetName() == FString("PlayerControls"))
		{
			//loads world when player loads a save and if player is on a different world
			UGameplayStatics::OpenLevel(GetWorld(), *SpawnInfo.currentWorldName);
			player->newLevelLoaded = true;
			return false;
		}

		if (!APlayerControls::toNewWorld)
		{
			ActorOut->SetActorTransform(SpawnInfo.ActorTransform);
			ActorOut->Serialize(Ar);
			player->currentWorldName = SpawnInfo.currentWorldName;
		}

		if (!player->characterProfile) player->DispatchBeginPlay();

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
		player->characterProfile->currentInventoryWeight = SpawnInfo.currentInventoryWeight;



		//Character Stats
		player->characterProfile->charName = SpawnInfo.charName;

		player->characterProfile->charGender = static_cast<FCharacterGender>(SpawnInfo.charGender);
		player->characterProfile->charRace = static_cast<FCharacterRace>(SpawnInfo.charRace);
		player->characterProfile->charClass = static_cast<FCharacterClasses>(SpawnInfo.charClass);

		player->characterProfile->beginningStats = SpawnInfo.beginningStats;
		player->characterProfile->characterStats = SpawnInfo.characterStats;
		player->characterProfile->RefreshStats(); //Does stat calculations again, It saved my time by not saving max health and inventory value

		player->characterProfile->statPoints = SpawnInfo.statPoints;
		player->characterProfile->relationWithPlayer = SpawnInfo.relationWithPlayer;

		//Character Status
		player->characterProfile->characterCurrentHealth = SpawnInfo.characterCurrentHealth;

		//Group Info
		player->charIndex = SpawnInfo.charIndex;
		player->inGroup = SpawnInfo.inGroup;

		//Character Armor

		if (SpawnInfo.characterArmor.top.name != "")
		{
			SpawnInfo.characterArmor.top.texture = LoadObject<UTexture>(nullptr, *SpawnInfo.characterArmor.top.texturePath);
			SpawnInfo.characterArmor.top.skeletalMesh = LoadObject<USkeletalMesh>(nullptr, *SpawnInfo.characterArmor.top.skeletalMeshPath);

			player->characterProfile->characterArmor.top = SpawnInfo.characterArmor.top;
			player->torsoMesh->SetSkeletalMesh(player->characterProfile->characterArmor.top.skeletalMesh);
		}



		//After wearing everything, resetting animations stabilizes character animation
		player->ResetAnimations();

		//------------------------------------------------------------------------------Loading Done------------------------------------------------
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
	playerSave = playerSave->groupMembers[0];
	SwitchToMainCharacter(playerSave);
	//If it false it means game loads a new level. When level loaded it will run again and load save data.
	if (!LoadSaveFile(playerSave, playerSave->GetName().Left(playerSave->GetName().Len() - 4), SName, TSave)) return;

	TArray<AActor*> persistentLevelActors = GetWorld()->PersistentLevel->Actors;

	for (AActor* actor : level->GetLoadedLevel()->Actors)
	{
		if (actor->GetClass()->GetSuperClass()->GetName() == "BP_NPC_Management_C")
		{
			FString actorName = actor->GetName().Left(actor->GetName().Find("_C_"));
			AActor* newActor = GetWorld()->SpawnActor<ANPC_Management>(actor->GetClass(), FVector::ZeroVector, FRotator::ZeroRotator);
			
			if (LoadSaveFile(newActor, actorName, SName, TSave))
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
					if (NPCSave->toNewWorld) //Set NPCs locations behind the player if they are moving to new world
					{
						if (NPCSave->charIndex == 1)
						{
							NPCSave->SetActorLocation(NPCSave->GetPlayerBehindLocation(155, 290));
						}
						else if (NPCSave->charIndex == 2)
						{
							NPCSave->SetActorLocation(NPCSave->GetPlayerBehindLocation(200, 0));
						}
						else if (NPCSave->charIndex == 3)
						{
							NPCSave->SetActorLocation(NPCSave->GetPlayerBehindLocation(140, -305));
						}
					}
				}
				for (AActor* persistentActor : persistentLevelActors)
				{
					if (persistentActor && persistentActor->GetName().Left(persistentActor->GetName().Find("_C_")) == actorName)
					{
						persistentActor->Rename(*FString::Printf(TEXT("DestroyedObject_%d"), FMath::Rand()));
						persistentActor->Destroy();
						break;
					}
				}

				FString newName = *NPCSave->GetName().Left(NPCSave->GetName().Find("_C_")) + FString("_C_1");
				NPCSave->Rename(*newName);
			}
			else
			{
				newActor->Destroy();
			}
			
		}
	}
	if (TSave)
	{
		IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
		FString path = UKismetSystemLibrary::GetProjectSavedDirectory() + "Saves/TransportSave";
		FileManager.DeleteDirectoryRecursively(*path);
	}
	
	timesLoaded++;
	APlayerControls::toNewWorld = false;
	TSave = false;
	SName = "";
	UGameplayStatics::UnloadStreamLevel(GetWorld(), FName("SaveLevel"), FLatentActionInfo(), true);
}

void USaveSystem::SaveGame(FString saveName, bool transportSave)
{
	TArray<AActor*> persistentLevelActors = GetWorld()->PersistentLevel->Actors;

	for (AActor* actor : persistentLevelActors)
	{
		if (actor && 
			(actor->GetClass()->GetSuperClass()->GetName() == FString("BP_NPC_Management_C") 
				|| actor->GetClass()->GetSuperClass()->GetName() == FString("PlayerControls")))
		{
			CreateSaveFile(actor, actor->GetName().Left(actor->GetName().Len() - 4), saveName, transportSave);
		}
	}
}

void USaveSystem::LoadGame(FString saveName, bool transportSave)
{
	SName = saveName;
	TSave = transportSave;

	level = UGameplayStatics::GetStreamingLevel(GetWorld(), FName("SaveLevel"));
	if (!saveLevelSet)
	{
		level->OnLevelLoaded.AddDynamic(this, &USaveSystem::OnLevelLoad);
		saveLevelSet = true;
	}
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
