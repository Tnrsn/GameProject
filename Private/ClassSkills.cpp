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

bool UClassSkills::CanHit(ACharacter* player, AActor* enemy)
{
	APlayerControls* hitter = Cast<APlayerControls>(player);
	bool damageToHostile = isDamageToHostile(player);

	if (Cast<ANPC_Management>(enemy))
	{
		ANPC_Management* enemyNPC = Cast<ANPC_Management>(enemy);
		if (enemyNPC->NPCStyle == Hostile && damageToHostile)
		{
			enemyNPC->ApplyDamage(hitter->characterProfile->characterStats.strength * 2);
			return true;
		}
		else if (enemyNPC->inGroup && !damageToHostile)
		{
			enemyNPC->ApplyDamage(hitter->characterProfile->characterStats.strength * 2);
			return true;
		}
	}
	else if (!damageToHostile) //Hits to main character
	{
		APlayerControls* enemyNPC = Cast<APlayerControls>(enemy);
		enemyNPC->ApplyDamage(hitter->characterProfile->characterStats.strength * 2);
		return true;
	}
	return false;
}

void UClassSkills::SkillOne(TEnumAsByte<FCharacterClasses> charClass, ACharacter* player, FVector target, AActor* enemy)
{
	APlayerControls* plyr = Cast<APlayerControls>(player);

	if (plyr->characterProfile->characterCurrentEnergy < 15) return;

	if (charClass == Warrior)
	{
		UE_LOG(LogTemp, Warning, TEXT("Warrior: Charge"));

		if (!skillOneTargeting)
		{
			HandleAbilityAreaDecal(player, 4.5f, .5f);

			skillOneTargeting = true;
			return;
		}

		plyr->characterProfile->characterCurrentEnergy -= 15;
		Charge(player, target);
	}
	else if (charClass == Rogue)
	{
		UE_LOG(LogTemp, Warning, TEXT("Rogue: DualWield"));


		if (!skillOneTargeting)
		{
			HandleAbilityAreaDecal(player, .45f, 0.f);

			skillOneTargeting = true;
			return;
		}

		if (DualWield(player, enemy))
		{
			plyr->characterProfile->characterCurrentEnergy -= 15;
		}
	}
	else if (charClass == Mage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mage: Blitz"));

		if (!skillOneTargeting)
		{
			HandleAbilityAreaDecal(player, 4.5, 0.f);

			skillOneTargeting = true;
			return;
		}

		if (Blitz(player, enemy))
		{
			plyr->characterProfile->characterCurrentEnergy -= 15;
		}
	}

	plyr->characterProfile->StartRefillCooldown(plyr->GetWorld());
	skillOneTargeting = false;
}

void UClassSkills::SkillTwo(TEnumAsByte<FCharacterClasses> charClass, ACharacter* player, FVector target, AActor* enemy)
{
	APlayerControls* plyr = Cast<APlayerControls>(player);
	if (plyr->characterProfile->characterCurrentEnergy < 25) return;

	if (charClass == Warrior)
	{
		UE_LOG(LogTemp, Warning, TEXT("Warrior: Whirlwind"));

		if (!skillTwoTargeting)
		{
			HandleAbilityAreaDecal(player, 1.08f, 0.f);
			skillTwoTargeting = true;
			return;
		}

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
			HandleAbilityAreaDecal(player, 5.f, 2.25f);
			skillTwoTargeting = true;
			return;
		}
		if (SuperNova(player, target))
		{
			plyr->characterProfile->characterCurrentEnergy -= 25;
		}
	}

	plyr->characterProfile->StartRefillCooldown(plyr->GetWorld());
	skillTwoTargeting = false;
}

void UClassSkills::SkillThree(TEnumAsByte<FCharacterClasses> charClass, ACharacter* player, FVector target, AActor* enemy)
{
	APlayerControls* plyr = Cast<APlayerControls>(player);
	if (plyr->characterProfile->characterCurrentEnergy < 35) return;

	if (charClass == Warrior)
	{
		UE_LOG(LogTemp, Warning, TEXT("Warrior: Power Strike"));

		if (!skillThreeTargeting)
		{
			HandleAbilityAreaDecal(player, .45f, 0.f);
			skillThreeTargeting = true;
			return;
		}

		if (PowerStrike(player, enemy))
		{
			plyr->characterProfile->characterCurrentEnergy -= 35;
		}
	}
	else if (charClass == Rogue)
	{
		UE_LOG(LogTemp, Warning, TEXT("Rogue: Backstab"));

		if (!skillThreeTargeting)
		{
			HandleAbilityAreaDecal(player, 3.85f, 0.f);
			skillThreeTargeting = true;
			return;
		}

		if (BacksStab(player, enemy))
		{
			plyr->characterProfile->characterCurrentEnergy -= 35;
		}
	}
	else if (charClass == Mage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mage: Blink"));
		if (!skillThreeTargeting)
		{
			HandleAbilityAreaDecal(player, 5.f, 0.f);
			skillThreeTargeting = true;
			return;
		}

		if (Blink(player, target))
		{
			plyr->characterProfile->characterCurrentEnergy -= 35;
		}
	}

	plyr->characterProfile->StartRefillCooldown(plyr->GetWorld());
	skillThreeTargeting = false;
}


//Warrior Skills
void UClassSkills::Charge(ACharacter* player, FVector target)
{
	//Disables Friction so character can slide on ground
	player->GetCharacterMovement()->GroundFriction = .0f;
	UWorld* world = player->GetWorld();

	// Get the current location of the character
	currentLocation = player->GetActorLocation();
	
	//Set DamageZone
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//ADamageZone* damageZone = player->GetWorld()->SpawnActor<ADamageZone>(ADamageZone::StaticClass(), currentLocation, FRotator::ZeroRotator, SpawnParams);
	ADamageZone* damageZone = world->SpawnActor<ADamageZone>(ADamageZone::StaticClass(), currentLocation, FRotator::ZeroRotator, SpawnParams);

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

	if (world)
	{
		float deltaSeconds = world->GetDeltaSeconds();
	 
	// //**********************************************************************************************
		world->GetTimerManager().SetTimer(chargeTimer, [&, world, hitter, player, damageZone]() 
			{
				currentLocation = player->GetActorLocation();
				damageZone->SetActorLocation(currentLocation);
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Hey"));
				//Sets Friction to default value if character speed is normal or less than normal
				if (player->GetVelocity().Size() <= 8500 || FVector::Distance(currentLocation, startLocation) > 1000.f)
				{
					player->GetCharacterMovement()->GroundFriction = 8.0f;

					damageZone->Destroy();




					hitter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
					//Stop Animation*******
					hitter->slashAnim = false;

				//	//Stops loop
					world->GetTimerManager().ClearTimer(chargeTimer);
				}
			}, deltaSeconds, true);
	}
}

void UClassSkills::WhirlWind(ACharacter* player)
{
	currentLocation = player->GetActorLocation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ADamageZone* damageZone = player->GetWorld()->SpawnActor<ADamageZone>(ADamageZone::StaticClass(), currentLocation, FRotator::ZeroRotator, SpawnParams);
	if (damageZone)
	{
		damageZone->damageArea->SetSphereRadius(125);
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

bool UClassSkills::PowerStrike(ACharacter* player, AActor* enemy)
{
	currentLocation = player->GetActorLocation();

	APlayerControls* hitter = Cast<APlayerControls>(player);
	
	if (Cast<APlayerControls>(enemy) && hitter->GetDistanceTo(enemy) <= 120)
	{
		if (CanHit(hitter, enemy))
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
			return true;
		}
		return false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PowerStrike: Too Far!"));
		return false;
	}
}

//Rogue Skills
bool UClassSkills::DualWield(ACharacter* player, AActor* enemy)
{
	currentLocation = player->GetActorLocation();

	APlayerControls* hitter = Cast<APlayerControls>(player);

	if (Cast<APlayerControls>(enemy) && hitter->GetDistanceTo(enemy) <= 120)
	{
		if (CanHit(hitter, enemy))
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
			return true;
		}
		return false;
	}
	else //It falls in else if character is too far away from the enemy or there are no enemy
	{
		UE_LOG(LogTemp, Warning, TEXT("DualWield: Too Far!"));
		return false;
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

bool UClassSkills::BacksStab(ACharacter* player, AActor* enemy)
{
	currentLocation = player->GetActorLocation();

	APlayerControls* hitter = Cast<APlayerControls>(player);

	if (Cast<APlayerControls>(enemy) && hitter->GetDistanceTo(enemy) <= 1000)
	{
		if (CanHit(hitter, enemy))
		{
			APlayerControls* enemyNPC = Cast<APlayerControls>(enemy);
			hitter->SetActorLocation(enemy->GetActorLocation() + (enemy->GetActorForwardVector() * -25));
			enemyNPC->ApplyDamage(hitter->characterProfile->characterStats.strength * 2);
			//Play Animation

			hitter->slashAnim = true;

			float TimeDilation = UGameplayStatics::GetGlobalTimeDilation(player->GetWorld());
			float DelayTime = player->GetWorld()->GetDeltaSeconds() / TimeDilation;

			player->GetWorldTimerManager().SetTimer(chargeTimer,
			FTimerDelegate::CreateLambda([=]()
				{
					hitter->slashAnim = false;
				}), DelayTime, false);
			return true;
		}
		return false;
	}
	else //It falls in else if character is too far away from the enemy or there are no enemy
	{
		UE_LOG(LogTemp, Warning, TEXT("BackStab: Too Far!"));
		return false;
	}
}

bool UClassSkills::Blitz(ACharacter* player, AActor* enemy)
{
	currentLocation = player->GetActorLocation();

	APlayerControls* hitter = Cast<APlayerControls>(player);

	if (Cast<APlayerControls>(enemy) && hitter->GetDistanceTo(enemy) <= 1100)
	{
		if (CanHit(hitter, enemy))
		{
			APlayerControls* enemyNPC = Cast<APlayerControls>(enemy);
			enemyNPC->ApplyDamage(hitter->characterProfile->characterStats.strength * 3);
			//Play Animation

			hitter->spellCasting = true;

			PlaySparks(player, enemyNPC->GetActorLocation());

			float TimeDilation = UGameplayStatics::GetGlobalTimeDilation(player->GetWorld());
			float DelayTime = player->GetWorld()->GetDeltaSeconds() / TimeDilation;

			player->GetWorldTimerManager().SetTimer(chargeTimer,
			FTimerDelegate::CreateLambda([=]()
			{
				hitter->spellCasting = false;
			}), DelayTime, false);
			return true;
		}
		return false;
	}
	else //It falls in else if character is too far away from the enemy or there are no enemy
	{
		return false;
		UE_LOG(LogTemp, Warning, TEXT("Blitz: Too Far!"));
	}

}

bool UClassSkills::SuperNova(ACharacter* player, FVector target)
{
	//Set DamageZone
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (FVector::Distance(player->GetActorLocation(), target) > 1100)
	{
		UE_LOG(LogTemp, Warning, TEXT("Too far!: SuperNova"));
		return false;
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

	return true;
}

bool UClassSkills::Blink(ACharacter* player, FVector target)
{
	//Set DamageZone
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (FVector::Distance(player->GetActorLocation(), target) > 1100)
	{
		UE_LOG(LogTemp, Warning, TEXT("Too far!: Blink"));
		return false;
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

	return true;
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

void UClassSkills::HandleAbilityAreaDecal(ACharacter* player, float abilityAreaSize, float selectionAreaSize)
{
	APlayerControls* playerCharacter = Cast<APlayerControls>(player);

	UWorld* world = playerCharacter->GetWorld();
	UDefaultGameInstance* instance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(world));
	abilityAreaDecal = world->SpawnActor<AAbilityDecal>(instance->abilityArea, playerCharacter->GetActorLocation(), FRotator::ZeroRotator);
	selectionDecal = world->SpawnActor<AAbilityDecal>(instance->selectionArea, playerCharacter->GetActorLocation(), FRotator::ZeroRotator);

	abilityAreaDecal->SetActorScale3D(FVector(6, abilityAreaSize, abilityAreaSize));
	selectionDecal->SetActorScale3D(FVector(6, selectionAreaSize, selectionAreaSize));

	/*FTimerHandle decalTimer;*/
	float deltaSeconds = world->GetDeltaSeconds();

	world->GetTimerManager().SetTimer(decalTimer, [&, playerCharacter, this, world]() {
		// Destroys decals after skill used

		//UE_LOG(LogTemp, Warning, TEXT("test"));
		if ((!skillOneTargeting && !skillTwoTargeting && !skillThreeTargeting && decalTimer.IsValid()) || playerCharacter->characterProfile->dead)
		{
			CancelSkillTargetings();
			DestroyDecals();

			//if (selectionDecal)
			//{
			//	selectionDecal->Destroy();
			//}
			//if (abilityAreaDecal)
			//{
			//	abilityAreaDecal->Destroy();
			//}
			if (world && decalTimer.IsValid())
			{
				
				//decalTimer.Invalidate();
				world->GetTimerManager().ClearTimer(decalTimer);
			}
			return;
		}

		// Sets ability area position
		if (abilityAreaDecal)
		{
			abilityAreaDecal->SetActorLocation(playerCharacter->GetActorLocation());
		}

		// Get the screen position of the mouse
		FHitResult HitResult;

		//UE_LOG(LogTemp, Warning, TEXT("test"));
		if (playerCharacter)
		{
			playerCharacter->playerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, HitResult);
		}
		if (HitResult.IsValidBlockingHit() && selectionDecal)
		{
			if (HitResult.GetActor() == playerCharacter)
			{
				// No hit result found for controlled character, try again with a different collision channel
				playerCharacter->playerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Camera, false, HitResult);
			}
			selectionDecal->SetActorLocation(HitResult.Location);
		}
		return;
	}, deltaSeconds, true);
}

void UClassSkills::CancelSkillTargetings()
{
	skillOneTargeting = false;
	skillTwoTargeting = false;
	skillThreeTargeting = false;
}

void UClassSkills::DestroyDecals()
{
	if (abilityAreaDecal)
	{
		abilityAreaDecal->Destroy();
	}
	if (selectionDecal)
	{
		selectionDecal->Destroy();
	}

	if (owner->GetWorld() && decalTimer.IsValid())
	{
		owner->GetWorld()->GetTimerManager().ClearTimer(decalTimer);
	}
}
