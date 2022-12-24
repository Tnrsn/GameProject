// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Components/CapsuleComponent.h>
#include "ManageWidgets.h"
#include "NPCDialogSystem.h"
#include "GameFramework/Actor.h"
#include "NPCManagement.generated.h"

UCLASS()
class GAMEPROJECT_API ANPCManagement : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANPCManagement();

	//Components
	UPROPERTY(VisibleAnywhere)
		USkeletalMeshComponent* SkeletalMesh;
	UPROPERTY(VisibleAnywhere)
		UCapsuleComponent* capsuleComponent;


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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
		void StartDialog();

};
