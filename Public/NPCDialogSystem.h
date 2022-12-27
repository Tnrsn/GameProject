// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ManageWidgets.h"
#include "UObject/NoExportTypes.h"
#include "NPCDialogSystem.generated.h"

/**
 * 
 */
UENUM(BlueprintType, Category = "Dialog System")
enum FAnswerType
{
	Neutral,
	EndDialog,
	Fight,
	JoinToPlayer,
	ChangeRelation
};
USTRUCT(BlueprintType)
struct FDialogAnswer
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dialog System")
		FString Answer;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dialog System")
		int NPCReplyIndex = 0;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dialog System")
		int newConversationIndex = -1;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dialog System")
		TEnumAsByte<FAnswerType> Type;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dialog System")
		int relationEffect = 0;
};
USTRUCT(BlueprintType)
struct FDialogTalk
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dialog System")
		FString NPCTalk;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dialog System")
		TArray<FDialogAnswer> Answer;
};
USTRUCT(BlueprintType)
struct FConversations
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TArray<FDialogTalk> Talk;
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

	//Function
	UFUNCTION()
		void EnableDialogUI(TSubclassOf<UUserWidget> dialogBoxUI, AActor* actor);
	UFUNCTION()
		void DisableDialogUI();
	UFUNCTION(BlueprintCallable)
		void RefreshDialogUI(TSubclassOf<UUserWidget> dialogBoxUI, AActor* actor);

};
