// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PlayerControls.h"
#include "NPC_Management.h"
#include "DefaultGameInstance.h"
#include <Components/BoxComponent.h>
#include "GameFramework/Actor.h"
#include "DialogTrigger.generated.h"

UCLASS()
class GAMEPROJECT_API ADialogTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADialogTrigger();

	UPROPERTY(EditAnywhere, Category = "Trigger Options")
		UBoxComponent* dialogTriggerBox;

	UPROPERTY(EditAnywhere, Category = "Dialog Options")
		FString dialogStarterCharName;
	UPROPERTY(EditAnywhere, Category = "Dialog Options")
		int conversationIndex;
	UPROPERTY(EditAnywhere, Category = "Dialog Options")
		bool inGroupConversation = false;

	UPROPERTY()
		UDefaultGameInstance* instance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
