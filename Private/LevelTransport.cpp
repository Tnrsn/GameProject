// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTransport.h"
#include "../SaveSystem.h"
#include <Kismet/GameplayStatics.h>
#include "../PlayerControls.h"

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
	USaveSystem* saveSystem;
	saveSystem = GetWorld()->GetSubsystem<USaveSystem>();

	APlayerControls* player = Cast<APlayerControls>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
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
		player->InitCharacter();
		player->inMenu = false;
	}


	APlayerControls::toNewWorld = true;
	saveSystem->SaveGame("", true);

	UGameplayStatics::OpenLevel(player->GetWorld(), worldName);
}

