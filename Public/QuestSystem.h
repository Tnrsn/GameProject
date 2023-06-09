// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuestSystem.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum FMainQuestLine
{
	None,
	TalkToElder,
	RescueTheGirl
};
UCLASS()
class GAMEPROJECT_API UQuestSystem : public UObject
{
	GENERATED_BODY()
	
public:
	UQuestSystem();

	//TArray<bool> mainQuestLine;
	bool mainQuestLine[5];
};
