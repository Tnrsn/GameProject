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
	APlayerControls* plyr = Cast<APlayerControls>(player);

	if (plyr->characterProfile->characterCurrentEnergy < 15) return;

	if (charClass == Warrior)
	{
		UE_LOG(LogTemp, Warning, TEXT("Warrior: Charge"));

		if (!skillOneTargeting)
		{
			skillOneTargeting = true;
			return;
		}

		plyr->characterProfile->characterCurrentEnergy -= 15;

		Charge(player, target);
	}
	else if (charClass == Rogue)
	{
		UE_LOG(LogTemp, Warning, TEXT("Rogue: DualWield"));

		plyr->characterProfile->characterCurrentEnergy -= 15;
		DualWield(player);
	}
	else if (charClass == Mage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mage: Blitz"));

		plyr->characterProfile->characterCurrentEnergy -= 15;
		Blitz(player);
	}

	plyr->characterProfile->StartRefillCooldown(plyr->GetWorld());
	skillOneTargeting = false;
}

void UClassSkills::SkillTwo(TEnumAsByte<FCharacterClasses> charClass, ACharacter* player, FVector target)
{
	APlayerControls* plyr = Cast<APlayerControls>(player);
	if (plyr->characterProfile->characterCurrentEnergy < 25) return;

	if (charClass == Warrior)
	{
		UE_LOG(LogTemp, Warning, TEXT("Warrior: Whirlwind"));

		plyr->characterProfile->characterCurrentEnergy -= 25;
		WhirlWind(player);
	}
	else if (charClass == Rogue)
	{
		UE_LOG(LogTemp, Warning, TEXT("Rogue: Evasion"));

		plyr->characterProfile->characterCurrentEnergy -= 25;
		Evasion(player);
	}
	else if (charClass == Mage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mage: Supernova"));
		if (!skillTwoTargeting)
		{
			skillTwoTargeting = true;
			return;
		}

		plyr->characterProfile->characterCurrentEnergy -= 25;
		SuperNova(player, target);
	}

	plyr->characterProfile->StartRefillCooldown(plyr->GetWorld());
	skillTwoTargeting = false;
}

void UClassSkills::SkillThree(TEnumAsByte<FCharacterClasses> charClass, ACharacter* player, FVector target)
{
	APlayerControls* plyr = Cast<APlayerControls>(player);
	if (plyr->characterProfile->characterCurrentEnergy < 35) return;

	if (charClass == Warrior)
	{
		UE_LOG(LogTemp, Warning, TEXT("Warrior: Power Strike"));

		plyr->characterProfile->characterCurrentEnergy -= 35;
		PowerStrike(player);
	}
	else if (charClass == Rogue)
	{
		UE_LOG(LogTemp, Warning, TEXT("Rogue: Backstab"));

		plyr->characterProfile->characterCurrentEnergy -= 35;
		BacksStab(player);
	}
	else if (charClass == Mage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mage: Blink"));
		if (!skillThreeTargeting)
		{
			skillThreeTargeting = true;
			return;
		}

		plyr->characterProfile->characterCurrentEnergy -= 35;
		Blink(player, target);
	}

	plyr->characterProfile->StartRefillCooldown(plyr->GetWorld());
	skillThreeTargeting = false;
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
	hitter->slashAnim = true;

	//float TimeDilation = UGameplayStatics::GetGlobalTimeDilation(player->GetWorld());
	//float DelayTime = player->GetWorld()->GetDeltaSeconds() / TimeDilation;

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
					hitter->slashAnim = false;
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
	APlayerControls* hitter = Cast<APlayerControls>(player);
	hitter->slashAnim = true;

	float TimeDilation = UGameplayStatics::GetGlobalTimeDilation(player->GetWorld());
	float DelayTime = player->GetWorld()->GetDeltaSeconds() * 5 / TimeDilation;

	player->GetWorldTimerManager().SetTimer(chargeTimer,
		FTimerDelegate::CreateLambda([=]()
			{
				damageZone->SetActorLocation(currentLocation);
				damageZone->Destroy();

				//Stop Animation
				hitter->slashAnim = false;
			}), DelayTime, false);
}

void UClassSkills::PowerStrike(ACharacter* player)
{
	FVector currentLocation = player->GetActorLocation();

	APlayerControls* hitter = Cast<APlayerControls>(player);
	
	if (Cast<APlayerControls>(hitter->actorToBeGone) && hitter->GetDistanceTo(hitter->actorToBeGone) <= 120)
	{
		if (CanHit(hitter))
		{
			hitter->slashAnim = true;

			float TimeDilation = UGameplayStatics::GetGlobalTimeDilation(player->GetWorld());
			float DelayTime = player->GetWorld()->GetDeltaSeconds() / TimeDilation;

			player->GetWorldTimerManager().SetTimer(chargeTimer,
				FTimerDelegate::CreateLambda([=]()
					{
						hitter->slashAnim = false;
					}), DelayTime, false);
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
			hitter->slashAnim = true;

			float TimeDilation = UGameplayStatics::GetGlobalTimeDilation(player->GetWorld());
			float DelayTime = player->GetWorld()->GetDeltaSeconds() / TimeDilation;

			player->GetWorldTimerManager().SetTimer(chargeTimer,
			FTimerDelegate::CreateLambda([=]()
				{
					hitter->slashAnim = false;
				}), DelayTime, false);
			
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
	Defender->defending = true;
	
	float TimeDilation = UGameplayStatics::GetGlobalTimeDilation(player->GetWorld());
	float DelayTime = player->GetWorld()->GetDeltaSeconds() * 10 / TimeDilation;

	player->GetWorldTimerManager().SetTimer(chargeTimer,
		FTimerDelegate::CreateLambda([=]()
			{
				Defender->defending = false;
				Defender->characterProfile->characterStats.dexterity = charDex; //Return dexterity to default value
				evasionActivated = false;
			}), DelayTime, false);
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

			hitter->slashAnim = true;

			float TimeDilation = UGameplayStatics::GetGlobalTimeDilation(player->GetWorld());
			float DelayTime = player->GetWorld()->GetDeltaSeconds() / TimeDilation;

			player->GetWorldTimerManager().SetTimer(chargeTimer,
			FTimerDelegate::CreateLambda([=]()
				{
					hitter->slashAnim = false;
				}), DelayTime, false);
			
		}
	}
	else //It falls in else if character is too far away from the enemy or there are no enemy
	{
		UE_LOG(LogTemp, Warning, TEXT("BackStab: Too Far!"));
	}
}

void UClassSkills::Blitz(ACharacter* player)
{
	FVector currentLocation = player->GetActorLocation();

	APlayerControls* hitter = Cast<APlayerControls>(player);

	if (Cast<APlayerControls>(hitter->actorToBeGone) && hitter->GetDistanceTo(hitter->actorToBeGone) <= 1100)
	{
		if (CanHit(hitter))
		{
			APlayerControls* enemy = Cast<APlayerControls>(hitter->actorToBeGone);
			enemy->ApplyDamage(hitter->characterProfile->characterStats.strength * 3);
			//Play Animation

			hitter->spellCasting = true;

			PlaySparks(player, enemy->GetActorLocation());

			float TimeDilation = UGameplayStatics::GetGlobalTimeDilation(player->GetWorld());
			float DelayTime = player->GetWorld()->GetDeltaSeconds() / TimeDilation;

			player->GetWorldTimerManager().SetTimer(chargeTimer,
			FTimerDelegate::CreateLambda([=]()
			{
				hitter->spellCasting = false;
			}), DelayTime, false);
		}
	}
	else //It falls in else if character is too far away from the enemy or there are no enemy
	{
		UE_LOG(LogTemp, Warning, TEXT("Blitz: Too Far!"));
	}

}

void UClassSkills::SuperNova(ACharacter* player, FVector target)
{
	//Set DamageZone
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (FVector::Distance(player->GetActorLocation(), target) > 1100)
	{
		UE_LOG(LogTemp, Warning, TEXT("Too far!: SuperNova"));
		return;
	}

	ADamageZone* damageZone = player->GetWorld()->SpawnActor<ADamageZone>(ADamageZone::StaticClass(), target, FRotator::ZeroRotator, SpawnParams);
	if (damageZone)
	{
		damageZone->damageArea->SetSphereRadius(600);
		damageZone->damage = 25;

		damageZone->damageToHostile = isDamageToHostile(player);
	}

	APlayerControls* hitter = Cast<APlayerControls>(player);
	hitter->spellCasting = true;
	
	PlaySparks(player, damageZone->GetActorLocation(), FVector(10));
	
	float TimeDilation = UGameplayStatics::GetGlobalTimeDilation(player->GetWorld());
	float DelayTime = player->GetWorld()->GetDeltaSeconds() * 5 / TimeDilation;

	player->GetWorldTimerManager().SetTimer(chargeTimer,
		FTimerDelegate::CreateLambda([=]()
			{
				hitter->spellCasting = false;
				damageZone->Destroy();
			}), DelayTime, false);
}

void UClassSkills::Blink(ACharacter* player, FVector target)
{
	//Set DamageZone
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (FVector::Distance(player->GetActorLocation(), target) > 1100)
	{
		UE_LOG(LogTemp, Warning, TEXT("Too far!: Blink"));
		return;
	}

	APlayerControls* hitter = Cast<APlayerControls>(player);
	hitter->spellCasting = true;

	float TimeDilation = UGameplayStatics::GetGlobalTimeDilation(player->GetWorld());
	float DelayTime = player->GetWorld()->GetDeltaSeconds() * 25 / TimeDilation;

	player->GetWorldTimerManager().SetTimer(chargeTimer,
		FTimerDelegate::CreateLambda([=]()
			{
				ADamageZone* damageZone = player->GetWorld()->SpawnActor<ADamageZone>(ADamageZone::StaticClass(), player->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
				if (damageZone)
				{
					damageZone->damageArea->SetSphereRadius(150);
					damageZone->damage = 25;

					damageZone->damageToHostile = isDamageToHostile(player);
				}
				PlaySparks(player, player->GetActorLocation());

				hitter->spellCasting = false;
				damageZone->Destroy();
				player->SetActorLocation(FVector(target.X, target.Y, target.Z + 50));

				PlaySparks(player, player->GetActorLocation());
			}), DelayTime, false);
}

void UClassSkills::PlaySparks(ACharacter* player, FVector location, FVector scale)
{
	APlayerControls* character = Cast<APlayerControls>(player);
	UParticleSystemComponent* sparkComp = UGameplayStatics::SpawnEmitterAtLocation(character->GetWorld(), character->sparks_F, location);

	sparkComp->SetRelativeScale3D(scale);

	float TimeDilation = UGameplayStatics::GetGlobalTimeDilation(player->GetWorld());
	float DelayTime = character->GetWorld()->GetDeltaSeconds() * 1.5f / TimeDilation;

	FTimerHandle vfxTimer;
	character->GetWorldTimerManager().SetTimer(vfxTimer,
		FTimerDelegate::CreateLambda([=]()
			{
				sparkComp->Deactivate();
			}), DelayTime, false);
}


//UParticleSystemComponent* sparkComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), sparks_F, GetActorLocation());
//
//FTimerHandle vfxTimer;
//GetWorldTimerManager().SetTimer(vfxTimer,
//	FTimerDelegate::CreateLambda([=]()
//		{
//			sparkComp->Deactivate();
//		}), GetWorld()->GetDeltaSeconds() * 1.5f, false);