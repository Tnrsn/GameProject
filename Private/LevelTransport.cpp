// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTransport.h"
#include "../SaveSystem.h"
#include <Kismet/GameplayStatics.h>
#include "../PlayerControls.h"
#include "../DefaultGameMode.h"


// Sets default values
ALevelTransport::ALevelTransport()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	staticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	RootComponent = staticMesh;

	staticMesh->OnClicked.AddDynamic(this, &ALevelTransport::TransportCharacter);
}

// Called when the game starts or when spawned
void ALevelTransport::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALevelTransport::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelTransport::TransportCharacter(UPrimitiveComponent* ClickedComponent, FKey ButtonPressed)
{
	UWorld* world = GetWorld();



	USaveSystem* saveSystem;
	saveSystem = world->GetSubsystem<USaveSystem>();

	APlayerControls* player = Cast<APlayerControls>(UGameplayStatics::GetPlayerCharacter(world, 0));
	if (player->groupMembers.Num() != 0)
	{
		player = player->groupMembers[0];
		if (!player->questSystem->mainQuestLine[opensAfter])
		{
			return;
		}

		saveSystem->SwitchToMainCharacter(player);
	}
	else
	{
		player->InitCharacter(world);
		player->inMenu = false;
	}

	if (worldName == "MainMenu")
	{
		UGameplayStatics::OpenLevel(world, worldName);
		return;
	}

	player->currentWorldName = worldName.ToString();

	APlayerControls::loadAfterNewWorld = true;
	if (!player->groupMembers[0]->characterProfile->charName.IsEmpty() && *world->GetName() != FName("CharacterCreationMenu"))
	{
		saveSystem->SaveGame(player->groupMembers[0]->characterProfile->charName, false);
		saveSystem->SaveGame("", true);
	}
	else
	{
		saveSystem->SaveGame("", true);
	}

	UGameplayStatics::OpenLevel(world, worldName);

	//LoadingScreen(worldName.ToString());
}

//void ALevelTransport::LoadingScreen(FString LevelName)
//{
//	// Get the level's asset registry
//	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
//	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
//
//	// Retrieve all assets in the level
//	TArray<FAssetData> LevelAssets;
//	FARFilter Filter;
//	FString PackagePath = "/Content/Levels/" + LevelName;
//	Filter.PackagePaths.Add(*PackagePath); // Assuming all assets are located in "/Game/LevelName" directory
//	AssetRegistry.GetAssets(Filter, LevelAssets);
//
//	// Create an array to store the asset paths to load
//	TArray<FSoftObjectPath> AssetPathsToLoad;
//
//	// Add the asset paths to load
//	for (const FAssetData& AssetData : LevelAssets)
//	{
//		FSoftObjectPath AssetPath = AssetData.ToSoftObjectPath();
//		AssetPathsToLoad.Add(AssetPath);
//	}
//
//	// Request asynchronous loading of assets
//	StreamableManager.RequestAsyncLoad(AssetPathsToLoad, FStreamableDelegate());
//
//	// Display your loading screen
//	// Check if all assets have finished loading
//	bool AllAssetsLoaded = true;
//	for (const FSoftObjectPath& AssetPath : AssetPathsToLoad)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("loading"));
//		if (!StreamableManager.IsAsyncLoadComplete(AssetPath))
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Something went wrong"));
//			AllAssetsLoaded = false;
//			break;
//		}
//	}
//
//	if (AllAssetsLoaded)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Everything loaded"));
//		// All assets have finished loading, hide the loading screen
//		// ...
//	}
//}
//
