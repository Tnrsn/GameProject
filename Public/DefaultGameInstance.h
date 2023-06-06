// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityDecal.h"
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

	UPROPERTY(BlueprintReadOnly)
		bool showHealthBars = false;

	UPROPERTY(BlueprintReadOnly)
		AActor* possessedActor;

	UPROPERTY(EditAnywhere, Category = "Decals")
		TSubclassOf<AAbilityDecal> selectionArea;
	UPROPERTY(EditAnywhere, Category = "Decals")
		TSubclassOf<AAbilityDecal> abilityArea;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Skill")
		UTexture* W_Skill1;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Skill")
		UTexture* W_Skill2;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Skill")
		UTexture* W_Skill3;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Skill")
		UTexture* R_Skill1;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Skill")
		UTexture* R_Skill2;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Skill")
		UTexture* R_Skill3;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Skill")
		UTexture* M_Skill1;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Skill")
		UTexture* M_Skill2;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Skill")
		UTexture* M_Skill3;
};
