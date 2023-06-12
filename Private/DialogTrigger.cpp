// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogTrigger.h"

// Sets default values
ADialogTrigger::ADialogTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	dialogTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DialogTrigger"));
	RootComponent = dialogTriggerBox;

	dialogTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADialogTrigger::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ADialogTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	instance = Cast<UDefaultGameInstance>(GetGameInstance());
}

// Called every frame
void ADialogTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADialogTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && instance->possessedActor == OtherActor && *OtherComp->GetName() == FString("CollisionCylinder"))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Dialog Started %s"), *OtherComp->GetName());

		APlayerControls* player = Cast<APlayerControls>(instance->possessedActor);

		if (inGroupConversation)
		{
			for (int i = 0; i < player->groupMembers.Num(); i++)
			{
				if (player->groupMembers[i]->characterProfile->charName != "" && player->groupMembers[i]->characterProfile->charName == dialogStarterCharName && Cast<ANPC_Management>(player->groupMembers[i]))
				{
					UE_LOG(LogTemp, Warning, TEXT("Dialog Started with %s"), *dialogStarterCharName);

					ANPC_Management* npc = Cast<ANPC_Management>(player->groupMembers[i]);
					npc->StartDialog(conversationIndex);

					//Destroy if triggering dialog is successfull
					Destroy();
					return;
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Dialog Started with %s"), *dialogStarterCharName);

			TArray<AActor*> npcs;
			UGameplayStatics::GetAllActorsOfClass(player->GetWorld(), ANPC_Management::StaticClass(), npcs);

			for (AActor* actor : npcs)
			{
				ANPC_Management* npc = Cast<ANPC_Management>(actor);
				if (npc->characterProfile->charName == dialogStarterCharName)
				{
					npc->StartDialog(conversationIndex);
					Destroy();
					return;
				}
			}
		}
	}
}
