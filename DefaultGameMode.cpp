// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameMode.h"

ADefaultGameMode::ADefaultGameMode()
{

}

ADefaultGameMode* ADefaultGameMode::GetDefaultObject()
{
	return Cast<ADefaultGameMode>(ADefaultGameMode::StaticClass()->GetDefaultObject());
}
