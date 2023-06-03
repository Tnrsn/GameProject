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
	

	ActorData.firstEncounter = player->firstEncounter;

	//Group info
	//ActorData.controlledCharIndex = player->controlledCharIndex;

	ActorData.charIndex = player->charIndex;
	ActorData.inGroup = player->inGroup;

	//Characte Armor
	ActorData.characterArmor.head = player->characterProfile->characterArmor.head;
	ActorData.characterArmor.top = player->characterProfile->characterArmor.top;
	ActorData.characterArmor.hand = player->characterProfile->characterArmor.hand;
	ActorData.characterArmor.foot = player->characterProfile->characterArmor.foot;
	ActorData.characterArmor.firstRing = player->characterProfile->characterArmor.firstRing;
	ActorData.characterArmor.secondRing = player->characterProfile->characterArmor.secondRing;
	ActorData.characterArmor.neck = player->characterProfile->characterArmor.neck;
	ActorData.characterArmor.weapon1 = player->characterProfile->characterArmor.weapon1;
	ActorData.characterArmor.weapon2 = player->characterProfile->characterArmor.weapon2;


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

				if (player->characterProfile->charGender == Male)
				{
					if (item.skeletalMeshPath_M != "")
					{
						item.skeletalMesh_M = LoadObject<USkeletalMesh>(nullptr, *item.skeletalMeshPath_M);
					}
				}
				else
				{
					if (item.skeletalMeshPath_F != "")
					{
						item.skeletalMesh_F = LoadObject<USkeletalMesh>(nullptr, *item.skeletalMeshPath_F);
					}
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
		player->characterProfile->RefreshStats(); //Does stat calculations again

		player->characterProfile->statPoints = SpawnInfo.statPoints;
		player->characterProfile->relationWithPlayer = SpawnInfo.relationWithPlayer;

		//Character Status
		player->characterProfile->characterCurrentHealth = SpawnInfo.characterCurrentHealth;

		//if (SpawnInfo.firstEncounter)
		//{
		player->firstEncounter = SpawnInfo.firstEncounter;
		UE_LOG(LogTemp, Warning, TEXT("%s teeeeeest 1"), *player->GetName());
		//}

		//Group Info
		player->charIndex = SpawnInfo.charIndex;
		player->inGroup = SpawnInfo.inGroup;

		if (player->characterProfile && *player->GetClass()->GetSuperClass()->GetName() == FString("PlayerControls"))
		{
			player->SetMesh();
		}

		//Character Armor
		LoadItem(player, SpawnInfo.characterArmor.head, player->characterProfile->characterArmor.head, player->headMesh);
		LoadItem(player, SpawnInfo.characterArmor.top, player->characterProfile->characterArmor.top, player->torsoMesh);
		LoadItem(player, SpawnInfo.characterArmor.hand, player->characterProfile->characterArmor.hand, player->handsMesh);
		LoadItem(player, SpawnInfo.characterArmor.foot, player->characterProfile->characterArmor.foot, player->footsMesh);
		LoadItem(player, SpawnInfo.characterArmor.firstRing, player->characterProfile->characterArmor.firstRing, nullptr);
		LoadItem(player, SpawnInfo.characterArmor.secondRing, player->characterProfile->characterArmor.secondRing, nullptr);
		LoadItem(player, SpawnInfo.characterArmor.neck, player->characterProfile->characterArmor.neck, nullptr);
		//Weapons will have a skeletalMesh soon for now Its nullptr
		LoadItem(player, SpawnInfo.characterArmor.weapon1, player->characterProfile->characterArmor.weapon1, nullptr, player->hand1);
		LoadItem(player, SpawnInfo.characterArmor.weapon2, player->characterProfile->characterArmor.weapon2, nullptr, player->hand2);

		player->firstArmorsEquipped = true;

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
		if (actor && actor->GetClass()->GetSuperClass()->GetName() == "BP_NPC_Management_C")
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

	ULevelStreaming* levelStreaming = GetWorld()->GetStreamingLevels()[0];
	
	
	//UGameplayStatics::UnloadStreamLevel(GetWorld(), levelStreaming->GetWorldAssetPackageFName(), FLatentActionInfo(), true);

	//UGameplayStatics::UnloadStreamLevel(GetWorld(), FName("SaveLevel"), FLatentActionInfo(), true);
	//FString Message = FString::Printf(TEXT("Level Name: %s"), *levelStreaming->GetWorldAssetPackageFName().ToString());
	//GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Red, Message, true);

	if (levelStreaming)
	{
		//levelStreaming->bShouldBeLoaded = false;
		//levelStreaming->bShouldBeVisible = false;
		levelStreaming->SetShouldBeLoaded(false);
		levelStreaming->SetShouldBeVisible(false);
	}
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

void USaveSystem::LoadItem(APlayerControls* player, FItemProperties& SpawnItem, FItemProperties& playerItem, USkeletalMeshComponent* skeletalMesh, UStaticMeshComponent* staticMeshComp)
{
	if (SpawnItem.name != "")
	{
		SpawnItem.texture = LoadObject<UTexture>(nullptr, *SpawnItem.texturePath);

		if (staticMeshComp != nullptr)
		{
			SpawnItem.staticMesh = LoadObject<UStaticMesh>(nullptr, *SpawnItem.staticMeshPath);
			staticMeshComp->SetStaticMesh(SpawnItem.staticMesh);
			staticMeshComp->SetRelativeLocation(SpawnItem.location);
			staticMeshComp->SetRelativeRotation(SpawnItem.rotation);
			staticMeshComp->SetRelativeScale3D(SpawnItem.scale);


			playerItem = SpawnItem;
			return;
		}

		if (player->characterProfile->charGender == Male)
		{
			SpawnItem.skeletalMesh_M = LoadObject<USkeletalMesh>(nullptr, *SpawnItem.skeletalMeshPath_M);

			playerItem = SpawnItem;
			if (skeletalMesh != nullptr)
			{
				//Player mesh required here (headMesh, topMesh etc.)
				skeletalMesh->SetSkeletalMesh(playerItem.skeletalMesh_M);
			}
		}
		else
		{
			SpawnItem.skeletalMesh_F = LoadObject<USkeletalMesh>(nullptr, *SpawnItem.skeletalMeshPath_F);

			playerItem = SpawnItem;
			if (skeletalMesh != nullptr)
			{
				//Player mesh required here (headMesh, topMesh etc.)
				skeletalMesh->SetSkeletalMesh(playerItem.skeletalMesh_F);
			}
		}
	}
}
