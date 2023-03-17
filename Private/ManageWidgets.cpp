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

float UManageWidgets::PlayerHealthBar()
{
	//playerCurrentHealth -= 0.1;
	return characterProfiles->characterCurrentHealth / characterProfiles->characterMaximumHealth;
}

float UManageWidgets::PlayerEnergyBar()
{
	return characterProfiles->characterCurrentEnergy / characterProfiles->characterMaximumEnergy;
}
