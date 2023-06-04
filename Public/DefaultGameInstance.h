// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DefaultGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UDefaultGameInstance : public UGameInstance
{
	GENERATED_BODY()
	

public:
	FString playerName;
	bool reloading = false;
};
