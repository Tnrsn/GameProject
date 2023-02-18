// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Components/StaticMeshComponent.h>
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void TransportCharacter(UPrimitiveComponent* ClickedComponent, FKey ButtonPressed);
};
