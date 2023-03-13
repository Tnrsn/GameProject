// Fill out your copyright notice in the Description page of Project Settings.


#include "ClassSkills.h"
#include <Engine/EngineTypes.h>
#include "../PlayerControls.h"
#include "NPC_Management.h"

bool UClassSkills::isDamageToHostile(ACharacter* player)
{
	if (Cast<ANPC_Management>(player))
	{
		ANPC_Management* hitter = Cast<ANPC_Management>(player);
		if (hitter->NPCStyle == Hostile)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return true;
	}
}

void UClassSkills::SkillOne(TEnumAsByte<FCharacterClasses> charClass, ACharacter* player, FVector target)
{
	if (charClass == Warrior)
	{
		UE_LOG(LogTemp, Warning, TEXT("Warrior: Charge"));
		Charge(player, target);
	}
	else if (charClass == Rogue)
	{
		UE_LOG(LogTemp, Warning, TEXT("Rogue"));
	}
	else if (charClass == Mage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mage"));
	}

	skillOneTargeting = false;
}

void UClassSkills::SkillTwo(TEnumAsByte<FCharacterClasses> charClass, ACharacter* player, FVector target)
{
	if (charClass == Warrior)
	{
		UE_LOG(LogTemp, Warning, TEXT("Warrior: Whirlwind"));
		WhirlWind(player);
	}
	else if (charClass == Rogue)
	{
		UE_LOG(LogTemp, Warning, TEXT("Rogue"));
	}
	else if (charClass == Mage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mage"));
	}
}

void UClassSkills::SkillThree(TEnumAsByte<FCharacterClasses> charClass)
{
	if (charClass == Warrior)
	{
		UE_LOG(LogTemp, Warning, TEXT("Warrior"));
	}
	else if (charClass == Rogue)
	{
		UE_LOG(LogTemp, Warning, TEXT("Rogue"));
	}
	else if (charClass == Mage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mage"));
	}
}

void UClassSkills::Charge(ACharacter* player, FVector target)
{
	//Disables Friction so character can slide on ground
	player->GetCharacterMovement()->GroundFriction = .0f;

	// Get the current location of the character
	FVector currentLocation = player->GetActorLocation();
	
	//Set DamageZone
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ADamageZone* damageZone = player->GetWorld()->SpawnActor<ADamageZone>(ADamageZone::StaticClass(), player->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
	if (damageZone)
	{
		damageZone->damageArea->SetSphereRadius(100);
		damageZone->damage = 25;

		damageZone->damageToHostile = isDamageToHostile(player);
	}

	APlayerControls* hitter = Cast<APlayerControls>(player);
	hitter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);

	//****************************************************************
	// Calculate the direction to move in
	FVector direction = target - currentLocation;
	direction.Z = 0.0f; // Ignore changes in elevation
	direction.Normalize(); // Normalize the direction vector to get a unit vector

	startLocation = player->GetActorLocation();
	
	// Set the movement input to move the character in the desired direction
	player->GetCharacterMovement()->Launch(direction * 10000);

	player->GetWorldTimerManager().SetTimer(chargeTimer,
		FTimerDelegate::CreateLambda([=]()
			{
				damageZone->SetActorLocation(player->GetActorLocation());

				//Sets Friction to default value if character speed is normal or less than normal
				if (player->GetVelocity().Size() <= 8500 || FVector::Distance(player->GetActorLocation(), startLocation) > 1000.f)
				{
					player->GetCharacterMovement()->GroundFriction = 8.0f;

					damageZone->Destroy();
					hitter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
					//Stops loop
					player->GetWorldTimerManager().ClearTimer(chargeTimer);
				}
			}), player->GetWorld()->GetDeltaSeconds(), true);
}

void UClassSkills::WhirlWind(ACharacter* player)
{
	FVector currentLocation = player->GetActorLocation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ADamageZone* damageZone = player->GetWorld()->SpawnActor<ADamageZone>(ADamageZone::StaticClass(), player->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
	if (damageZone)
	{
		damageZone->damageArea->SetSphereRadius(250);
		damageZone->damage = 50;

		damageZone->damageToHostile = isDamageToHostile(player);
	}

	player->GetWorldTimerManager().SetTimer(chargeTimer,
		FTimerDelegate::CreateLambda([=]()
			{
				damageZone->SetActorLocation(player->GetActorLocation());
				damageZone->Destroy();
			}), player->GetWorld()->GetDeltaSeconds() * 5, false);
}
