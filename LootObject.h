// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Components/SphereComponent.h>
#include <Kismet/GameplayStatics.h>
#include "Public/ManageWidgets.h"
#include "GameFramework/Actor.h"
#include "LootObject.generated.h"

UCLASS()
class GAMEPROJECT_API ALootObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALootObject();

	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere)
		class USphereComponent* SphereCollision;
	UPROPERTY(EditAnywhere)
		FName type;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
		TSubclassOf<UUserWidget> LootUI;
	class UManageWidgets* HUD;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	void GetType();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
