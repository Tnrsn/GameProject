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

	mainHUD = CreateWidget<UManageWidgets>(UGameplayStatics::GetPlayerController(GetWorld(), 0), mainUI);
	mainHUD->characterProfiles = NewObject<UCharacterProfiles>();

	characterProfile = NewObject<UCharacterProfiles>();
	mainHUD->characterProfiles = characterProfile;

	characterProfile->characterStats = characterStats;
	characterProfile->beginningStats = characterProfile->characterStats;

	characterProfile->charGender = charGender;
	characterProfile->charRace = charRace;
	characterProfile->charClass = charClass;

	//Calculates Maximum Inventory Capacity
	characterProfile->maxInventoryCapacity = (characterProfile->beginningStats.strength * 10) + ((characterProfile->characterStats.strength - characterProfile->beginningStats.strength) * 2);
	//Calculates Maximum Health
	characterProfile->characterMaximumHealth = (characterProfile->beginningStats.constitution * 10) + ((characterProfile->characterStats.constitution - characterProfile->beginningStats.constitution) * 2);
	characterProfile->characterCurrentHealth = characterProfile->characterMaximumHealth;

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
}

bool ANPC_Management::DialogEffect(TEnumAsByte<FAnswerType> type, int relationEffect)
{
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
		dialogSystem->RefreshDialogUI(dialogBoxUI, this);
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
	else if (type == ChangeRelation)
	{
		characterProfile->relationWithPlayer = FMath::Clamp(characterProfile->relationWithPlayer + relationEffect, -100, 100);
		dialogSystem->RefreshDialogUI(dialogBoxUI, this);
		return true;
	}
	return false;
}