// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageZone.h"
#include "NPC_Management.h"

// Sets default values
ADamageZone::ADamageZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	damageArea = CreateDefaultSubobject<USphereComponent>(TEXT("Damage Area"));
	RootComponent = damageArea;
}

// Called when the game starts or when spawned
void ADamageZone::BeginPlay()
{
	Super::BeginPlay();
	
	damageArea->OnComponentBeginOverlap.AddDynamic(this, &ADamageZone::OnOverlapBegin);
}

// Called every frame
void ADamageZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!firstDamageGiven)
	{
		TArray<AActor*> actors;
		damageArea->GetOverlappingActors(actors);
		for (AActor* actor : actors)
		{
			if (actor->GetName() == "Find Enemy Component") continue;

			if (Cast<ANPC_Management>(actor))
			{
				ANPC_Management* enemy = Cast<ANPC_Management>(actor);
				if (enemy->NPCStyle == Hostile && damageToHostile) //Damages to hostile npcs
				{
					enemy->ApplyDamage(damage);
				}
				else if (enemy->inGroup && !damageToHostile) //Damages to players group
				{
					enemy->ApplyDamage(damage);
				}
			}
			else if(Cast<APlayerControls>(actor) && !damageToHostile) //Damages to player
			{
				APlayerControls* mainCharacter = Cast<APlayerControls>(actor);
				mainCharacter->ApplyDamage(damage);
			}
		}

		firstDamageGiven = true;
	}
}

void ADamageZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && Cast<APlayerControls>(OtherActor))
	{
		if (OtherComp->GetName() == "Find Enemy Component") return;

		if (Cast<ANPC_Management>(OtherActor))
		{
			ANPC_Management* enemy = Cast<ANPC_Management>(OtherActor);
			if (enemy->NPCStyle == Hostile && damageToHostile) //Damages to hostile npcs
			{
				enemy->ApplyDamage(damage);
			}
			else if (enemy->inGroup && !damageToHostile) //Damages to players group
			{
				enemy->ApplyDamage(damage);
			}
		}
		else if(!damageToHostile) //Damages to player
		{
			APlayerControls* mainCharacter = Cast<APlayerControls>(OtherActor);
			mainCharacter->ApplyDamage(damage);
		}
	}
}