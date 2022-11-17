// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Kismet/GameplayStatics.h>
#include <Components/SphereComponent.h>
#include <Kismet/GameplayStatics.h>
#include "Public/ManageWidgets.h"
#include <Components/Button.h>
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

	UPROPERTY(VisibleAnywhere)
		class USphereComponent* TriggerCollision;

	//UPROPERTY()
	//	APlayerController* playerController;

	UPROPERTY(EditAnywhere)
		FName type;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
		TSubclassOf<UUserWidget> LootUI;
	class UManageWidgets* HUD;

	bool lootUIEnabled = false;
	bool canOpenLoot = false;

	//2D position coordinates
	float X;
	float Y;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	void EnableLootUI();
	void DisableLootUI(AActor* SelectedActor);

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
