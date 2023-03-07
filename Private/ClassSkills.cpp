// Fill out your copyright notice in the Description page of Project Settings.


#include "ClassSkills.h"
#include "../PlayerControls.h"

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

void UClassSkills::SkillTwo(TEnumAsByte<FCharacterClasses> charClass)
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

void UClassSkills::SkillFour(TEnumAsByte<FCharacterClasses> charClass)
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

void UClassSkills::SkillFive(TEnumAsByte<FCharacterClasses> charClass)
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

	// Calculate the direction to move in
	FVector direction = target - currentLocation;
	direction.Z = 0.0f; // Ignore changes in elevation
	direction.Normalize(); // Normalize the direction vector to get a unit vector

	//APlayerControls* character = Cast<APlayerControls>(player);
	startLocation = player->GetActorLocation();
	
	// Set the movement input to move the character in the desired direction
	player->GetCharacterMovement()->Launch(direction * 10000);

	player->GetWorldTimerManager().SetTimer(chargeTimer,
		FTimerDelegate::CreateLambda([=]()
			{
				//UE_LOG(LogTemp, Warning, TEXT("%f"), player->GetVelocity().Size());
				//Sets Friction to default value if character speed is normal or less than normal
				if (player->GetVelocity().Size() <= 8500 || FVector::Distance(player->GetActorLocation(), startLocation) > 1000.f)
				{
					player->GetCharacterMovement()->GroundFriction = 8.0f;

					//Stops loop
					player->GetWorldTimerManager().ClearTimer(chargeTimer);
				}
			}), player->GetWorld()->GetDeltaSeconds(), true);
}
