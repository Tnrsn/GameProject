// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestSystem.h"
#include <Components/StaticMeshComponent.h>
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "GameFramework/Actor.h"
#include "LevelTransport.generated.h"


UCLASS()
class GAMEPROJECT_API ALevelTransport : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelTransport();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName worldName;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* staticMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Which Quest Opens The Door")
		TEnumAsByte<FMainQuestLine> opensAfter;

	FStreamableManager StreamableManager;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void TransportCharacter(UPrimitiveComponent* ClickedComponent, FKey ButtonPressed);
	UFUNCTION()
		void ResetTimers(UWorld* world);
	//UFUNCTION()
	//	void LoadingScreen(FString LevelName);
};
