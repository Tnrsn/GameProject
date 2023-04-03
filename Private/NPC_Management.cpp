// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC_Management.h"

ANPC_Management::ANPC_Management()
{

}

// Called when the game starts or when spawned
void ANPC_Management::BeginPlay()
{
	Super::BeginPlay();

	dialogSystem = NewObject<UNPCDialogSystem>();
	characterProfile = NewObject<UCharacterProfiles>();

	mainHUD->characterProfiles = NewObject<UCharacterProfiles>();
	mainHUD->characterProfiles = characterProfile;

	if (characterProfile)
	{
		characterProfile->characterStats = characterStats;
		characterProfile->beginningStats = characterProfile->characterStats;

		characterProfile->charGender = charGender;
		characterProfile->charRace = charRace;
		characterProfile->charClass = charClass;
		
		characterProfile->RefreshStats();
		characterProfile->characterCurrentHealth = characterProfile->characterMaximumHealth;
		characterProfile->characterCurrentEnergy = characterProfile->characterMaximumEnergy;
	}

	groupMembers.Reset();

}

// Called every frame
void ANPC_Management::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANPC_Management::StartDialog()
{
	playerControl = Cast<APlayerControls>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	playerControl->SelectedActor = this;
	playerControl->inDialog = true;
	dialogSystem->conversation = Conversations;
	dialogSystem->EnableDialogUI(dialogBoxUI, this);

	SetActorRotation((UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation() - GetActorLocation()).Rotation());
}

bool ANPC_Management::DialogEffect(TEnumAsByte<FAnswerType> type, int relationEffect, TEnumAsByte<FMainQuestLine> completedQuest)
{
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *StaticEnum<FMainQuestLine>()->GetValueAsString(completedQuest));
	//Sets true of the quest if it's completed
	APlayerControls* player = Cast<APlayerControls>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	player->groupMembers[0]->questSystem->mainQuestLine[completedQuest] = true;
	//groupMembers[0]->questSystem->mainQuestLine[completedQuest] = true;

	//Changes players relation with npcs
	characterProfile->relationWithPlayer = FMath::Clamp(characterProfile->relationWithPlayer + relationEffect, -100, 100);

	if (type == Neutral)
	{
		dialogSystem->RefreshDialogUI(dialogBoxUI, this);
		return true;
	}
	else if (type == EndDialog)
	{
		dialogSystem->DisableDialogUI();
		return false;
	}
	else if (type == Fight)
	{
		NPCStyle = Hostile;
		dialogSystem->RefreshDialogUI(dialogBoxUI, this);

		findEnemyComponent->nearbyEnemies.Empty();

		TArray<AActor*> actors;
		findEnemyComponent->GetOverlappingActors(actors);
		for (AActor* actor : actors)
		{
			OverlappedWithActor(actor);
		}

		return true;
	}
	else if (type == JoinToPlayer)
	{
		if (!playerControl->groupMembers.Contains(this))
		{
			controlledChar = playerControl->controlledChar;
			playerControl->groupMembers.Add(this);
			groupMembers = playerControl->groupMembers;
			inGroup = true;

			for (int i = 0; i <= groupMembers.Num() - 1; i++)
			{
				if (groupMembers[i] == this)
				{
					charIndex = i;
				}
			}
		}
		dialogSystem->RefreshDialogUI(dialogBoxUI, this);
		return true;
	}


	return false;
}