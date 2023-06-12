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
	UDefaultGameInstance* instance = Cast<UDefaultGameInstance>(GetGameInstance());

	APlayerControls* player = Cast<APlayerControls>(UGameplayStatics::GetPlayerCharacter(world, 0));
	
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("1"));

	if (player->skills->skillOneTargeting || player->skills->skillTwoTargeting || player->skills->skillThreeTargeting)
	{
		return;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("2"));
	if (player->groupMembers.Num() != 0)
	{
		player = player->groupMembers[0];
		if (opensAfter != FMainQuestLine::None && !player->questSystem->mainQuestLine[opensAfter])
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("3"));
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
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("4"));
		ResetTimers(world);
		UGameplayStatics::OpenLevel(world, worldName);
		return;
	}

	player->currentWorldName = worldName.ToString();

	//APlayerControls::loadAfterNewWorld = true;
	if (*world->GetName() == FName("CharacterCreationMenu"))
	{
		instance->playerName = player->groupMembers[0]->characterProfile->charName;
	}

	if (!player->groupMembers[0]->characterProfile->charName.IsEmpty())
	{
		saveSystem->SaveGame(player->GetWorld(), instance->playerName, false);
		instance->switchingAnotherWorld = true;
	}

	ResetTimers(world);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("5"));
	UGameplayStatics::OpenLevel(world, worldName);

}

void ALevelTransport::ResetTimers(UWorld* world)
{
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(world, APlayerControls::StaticClass(), actors);

	for (AActor* actor : actors)
	{
		if (actor && Cast<APlayerControls>(actor))
		{
			APlayerControls* character = Cast<APlayerControls>(actor);
			
			if (character->characterProfile->refillCooldownTimerHandle.IsValid())
			{
				world->GetTimerManager().ClearTimer(character->characterProfile->refillCooldownTimerHandle);
			}
			if (character->characterProfile->refillTimerHandle.IsValid())
			{
				world->GetTimerManager().ClearTimer(character->characterProfile->refillTimerHandle);
			}
			if (character->skills->chargeTimer.IsValid())
			{
				world->GetTimerManager().ClearTimer(character->skills->chargeTimer);
			}
		}
	}
}
