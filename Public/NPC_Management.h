// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPCDialogSystem.h"
#include "../PlayerControls.h"
#include "NPC_Management.generated.h"

/**
 * 
 */
UENUM(BlueprintType, Category = "NPC Style")
enum FNPCStyle
{
	NonTalkable,
	Talkable,
	Hostile
};
UCLASS()
class GAMEPROJECT_API ANPC_Management : public APlayerControls
{
	GENERATED_BODY()

public:
	ANPC_Management();

	UPROPERTY(EditAnywhere, Blueprintreadwrite)
		TEnumAsByte<FNPCStyle> NPCStyle;

	//Dialog System
	UPROPERTY(BlueprintReadOnly)
		UNPCDialogSystem* dialogSystem;
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
		TSubclassOf<UUserWidget> dialogBoxUI;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialog System")
		TArray<FConversations> Conversations;
	UPROPERTY(BlueprintReadWrite)
		int currentConservation = 0;
	UPROPERTY(BlueprintReadWrite)
		int currentTalk = 0;
	UPROPERTY(BlueprintReadWrite)
		int triggerConversationIndex = -2;

	APlayerControls* playerControl;

	//NPC stats
	UPROPERTY(EditAnywhere)
		FCharacterStats characterStats;
	UPROPERTY(EditAnywhere)
		TEnumAsByte<FCharacterRace> charRace;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TEnumAsByte<FCharacterGender> charGender;
	UPROPERTY(EditAnywhere)
		TEnumAsByte<FCharacterClasses> charClass;

	UPROPERTY(EditAnywhere)
		FString charName;
	//UPROPERTY(EditAnywhere)
	//	FCharacterWearables characterWearables;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void StartDialog(int conversationTriggerIndex = -2);
	UFUNCTION(BlueprintCallable)
		bool DialogEffect(TEnumAsByte<FAnswerType> type, int relationEffect, TEnumAsByte<FMainQuestLine> completedQuest, FString newName); //Returns if player still in a dialog
};
