// Fill out your copyright notice in the Description page of Project Settings.


#include "ManageWidgets.h"

UManageWidgets::UManageWidgets(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//characterProfiles = NewObject<UCharacterProfiles>();
}

void UManageWidgets::ButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Pressed"));
}

void UManageWidgets::ButtonReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("Released"));
}

float UManageWidgets::PlayerHealthBar(UCharacterProfiles* charProf)
{
	if (!characterProfiles && charProf)
	{
		characterProfiles = charProf;
	}
	else if (!characterProfiles) return .5f;

	return characterProfiles->characterCurrentHealth / characterProfiles->characterMaximumHealth;
}

float UManageWidgets::PlayerEnergyBar()
{
	if (!characterProfiles) return .5f;
	return characterProfiles->characterCurrentEnergy / characterProfiles->characterMaximumEnergy;
}

bool UManageWidgets::canShowHealthbar(AActor* actor)
{
	UDefaultGameInstance* instance = Cast<UDefaultGameInstance>(GetGameInstance());
	if (!actor) return false;
	if (!instance->possessedActor || actor == instance->possessedActor) return false;
	if (FVector::Distance(instance->possessedActor->GetActorLocation(), actor->GetActorLocation()) > 2200) return false;
	return true;
}
