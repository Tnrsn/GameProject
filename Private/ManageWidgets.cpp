// Fill out your copyright notice in the Description page of Project Settings.


#include "ManageWidgets.h"

UManageWidgets::UManageWidgets(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UManageWidgets::ButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Hey!"));
	testNum++;
}

void UManageWidgets::ButtonReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("Bye!"));
}
