// Fill out your copyright notice in the Description page of Project Settings.


#include "ManageWidgets.h"

UManageWidgets::UManageWidgets(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UManageWidgets::ButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Pressed"));
	counter0++;

	
}

void UManageWidgets::ButtonReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("Released"));
}

float UManageWidgets::PlayerHealthBar()
{
	//playerCurrentHealth -= 0.1;
	return playerCurrentHealth / playerMaximumHealth;
}