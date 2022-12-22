// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ManageWidgets.h"
#include "UObject/NoExportTypes.h"
#include "NPCDialogSystem.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDialogAnswer
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dialog System")
		FString NPCTalk;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dialog System")
		TArray<FString> Answer;
};
USTRUCT(BlueprintType)
struct FConversations
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TArray<FDialogAnswer> Talk;
};

UCLASS()
class GAMEPROJECT_API UNPCDialogSystem : public UObject
{
	GENERATED_BODY()
	
public:

	UNPCDialogSystem();

	UPROPERTY(BlueprintReadOnly)
		class UManageWidgets* dialogBox;

	//Dialog
	TArray<FConversations> conversation;

	int test = 5;

	//Function
	UFUNCTION()
		void EnableDialogBox(TSubclassOf<UUserWidget> dialogBoxUI, AActor* actor);

	UFUNCTION()
		void DisableDialogBox();
};
