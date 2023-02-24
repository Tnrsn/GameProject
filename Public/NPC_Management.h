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
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dialog System")
		TArray<FConversations> Conversations;
	UPROPERTY(BlueprintReadWrite)
		int currentConservation = 0;
	UPROPERTY(BlueprintReadWrite)
		int currentTalk = 0;

	APlayerControls* playerControl;

	//NPC stats
	UPROPERTY(EditAnywhere)
		FCharacterStats characterStats;
	UPROPERTY(EditAnywhere)
		TEnumAsByte<FCharacterRace> charRace;
	UPROPERTY(EditAnywhere)
		TEnumAsByte<FCharacterGender> charGender;
	UPROPERTY(EditAnywhere)
		TEnumAsByte<FCharacterClasses> charClass;
	UPROPERTY(EditAnywhere)
		FCharacterWearables characterWearables;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void StartDialog();
	UFUNCTION(BlueprintCallable)
		bool DialogEffect(TEnumAsByte<FAnswerType> type, int relationEffect); //Returns if player still in a dialog

};
