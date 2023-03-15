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

bool UClassSkills::CanHit(ACharacter* player)
{
	APlayerControls* hitter = Cast<APlayerControls>(player);
	bool damageToHostile = isDamageToHostile(player);

	if (Cast<ANPC_Management>(hitter->actorToBeGone))
	{
		ANPC_Management* enemy = Cast<ANPC_Management>(hitter->actorToBeGone);
		if (enemy->NPCStyle == Hostile && damageToHostile)
		{
			enemy->ApplyDamage(hitter->characterProfile->characterStats.strength * 2);
			return true;
		}
		else if (enemy->inGroup && !damageToHostile)
		{
			enemy->ApplyDamage(hitter->characterProfile->characterStats.strength * 2);
			return true;
		}
	}
	else if (!damageToHostile) //Hits to main character
	{
		APlayerControls* enemy = Cast<APlayerControls>(hitter->actorToBeGone);
		enemy->ApplyDamage(hitter->characterProfile->characterStats.strength * 2);
		return true;
	}
	return false;
}

void UClassSkills::SkillOne(TEnumAsByte<FCharacterClasses> charClass, ACharacter* player, FVector target)
{
	if (charClass == Warrior)
	{
		UE_LOG(LogTemp, Warning, TEXT("Warrior: Charge"));
		if (!skillOneTargeting)
		{
			skillOneTargeting = true;
			return;
		}

		Charge(player, target);
	}
	else if (charClass == Rogue)
	{
		UE_LOG(LogTemp, Warning, TEXT("Rogue: DualWield"));
		DualWield(player);
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
		UE_LOG(LogTemp, Warning, TEXT("Rogue: Evasion"));
		Evasion(player);
	}
	else if (charClass == Mage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mage"));
	}

	skillOneTargeting = false;
}

void UClassSkills::SkillThree(TEnumAsByte<FCharacterClasses> charClass, ACharacter* player, FVector target)
{
	if (charClass == Warrior)
	{
		UE_LOG(LogTemp, Warning, TEXT("Warrior: Power Strike"));
		PowerStrike(player);
	}
	else if (charClass == Rogue)
	{
		UE_LOG(LogTemp, Warning, TEXT("Rogue: Backstab"));
		BacksStab(player);
	}
	else if (charClass == Mage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mage"));
	}
}


//Warrior Skills
void UClassSkills::Charge(ACharacter* player, FVector target)
{
	//Disables Friction so character can slide on ground
	player->GetCharacterMovement()->GroundFriction = .0f;

	// Get the current location of the character
	FVector currentLocation = player->GetActorLocation();
	
	//Set DamageZone
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ADamageZone* damageZone = player->GetWorld()->SpawnActor<ADamageZone>(ADamageZone::StaticClass(), currentLocation, FRotator::ZeroRotator, SpawnParams);
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

	//Play Animation********

	player->GetWorldTimerManager().SetTimer(chargeTimer,
		FTimerDelegate::CreateLambda([=]()
			{
				FVector currentLocation = player->GetActorLocation();
				damageZone->SetActorLocation(currentLocation);

				//Sets Friction to default value if character speed is normal or less than normal
				if (player->GetVelocity().Size() <= 8500 || FVector::Distance(currentLocation, startLocation) > 1000.f)
				{
					player->GetCharacterMovement()->GroundFriction = 8.0f;

					damageZone->Destroy();
					hitter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
					//Stops loop
					player->GetWorldTimerManager().ClearTimer(chargeTimer);

					//Stop Animation*******
				}
			}), player->GetWorld()->GetDeltaSeconds(), true);
}

void UClassSkills::WhirlWind(ACharacter* player)
{
	FVector currentLocation = player->GetActorLocation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ADamageZone* damageZone = player->GetWorld()->SpawnActor<ADamageZone>(ADamageZone::StaticClass(), currentLocation, FRotator::ZeroRotator, SpawnParams);
	if (damageZone)
	{
		damageZone->damageArea->SetSphereRadius(250);
		damageZone->damage = 50;

		damageZone->damageToHostile = isDamageToHostile(player);
	}

	player->GetWorldTimerManager().SetTimer(chargeTimer,
		FTimerDelegate::CreateLambda([=]()
			{
				damageZone->SetActorLocation(currentLocation);
				damageZone->Destroy();

				//Play Animation
			}), player->GetWorld()->GetDeltaSeconds() * 5, false);
}

void UClassSkills::PowerStrike(ACharacter* player)
{
	FVector currentLocation = player->GetActorLocation();

	APlayerControls* hitter = Cast<APlayerControls>(player);
	
	if (Cast<APlayerControls>(hitter->actorToBeGone) && hitter->GetDistanceTo(hitter->actorToBeGone) <= 120)
	{
		if (CanHit(hitter))
		{
			//Play Animation
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PowerStrike: Too Far!"));
	}
}

//Rogue Skills
void UClassSkills::DualWield(ACharacter* player)
{
	FVector currentLocation = player->GetActorLocation();

	APlayerControls* hitter = Cast<APlayerControls>(player);

	if (Cast<APlayerControls>(hitter->actorToBeGone) && hitter->GetDistanceTo(hitter->actorToBeGone) <= 120)
	{
		if (CanHit(hitter))
		{
			//Play Animation
		}
	}
	else //It falls in else if character is too far away from the enemy or there are no enemy
	{
		UE_LOG(LogTemp, Warning, TEXT("DualWield: Too Far!"));
	}
}

void UClassSkills::Evasion(ACharacter* player)
{
	APlayerControls* Defender = Cast<APlayerControls>(player);
	
	if (evasionActivated) //Prevents to increase dexterity more than 15
	{
		Defender->characterProfile->characterStats.dexterity = charDex;
	}

	charDex = Defender->characterProfile->characterStats.dexterity;
	Defender->characterProfile->characterStats.dexterity += 15;
	evasionActivated = true;
	//Play Animation

	player->GetWorldTimerManager().SetTimer(chargeTimer,
		FTimerDelegate::CreateLambda([=]()
			{
				Defender->characterProfile->characterStats.dexterity = charDex; //Return dexterity to default value
				evasionActivated = false;
			}), player->GetWorld()->GetDeltaSeconds() * 10, false);
}

void UClassSkills::BacksStab(ACharacter* player)
{
	FVector currentLocation = player->GetActorLocation();

	APlayerControls* hitter = Cast<APlayerControls>(player);

	if (Cast<APlayerControls>(hitter->actorToBeGone) && hitter->GetDistanceTo(hitter->actorToBeGone) <= 1000)
	{
		if (CanHit(hitter))
		{
			APlayerControls* enemy = Cast<APlayerControls>(hitter->actorToBeGone);
			hitter->SetActorLocation(enemy->GetActorLocation() + (enemy->GetActorForwardVector() * -25));
			enemy->ApplyDamage(hitter->characterProfile->characterStats.strength * 2);
			//Play Animation
		}
	}
	else //It falls in else if character is too far away from the enemy or there are no enemy
	{
		UE_LOG(LogTemp, Warning, TEXT("BackStab: Too Far!"));
	}
}
