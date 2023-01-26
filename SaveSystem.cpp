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

	//ActorData.testt = player->testNum;

	ActorData.inventoryData = player->inventory;
	ActorData.characterHealth = player->characterProfile->characterCurrentHealth;
	ActorData.ActorTransform = Actor->GetTransform();
	ActorData.ptr = Actor->GetClass();

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

	ActorOut->SetActorTransform(SpawnInfo.ActorTransform);
	ActorOut->Serialize(Ar);

	//Player Datas

	APlayerControls* player = Cast<APlayerControls>(Actor);
	player->characterProfile->characterCurrentHealth = SpawnInfo.characterHealth;

	//player->inventory[0].name = SpawnInfo.inventoryData[0].name;


	//player->testNum = SpawnInfo.testt;


	//***********************
	FromBinary.FlushCache();
	BinaryArray.Empty();
	FromBinary.Close();

}
