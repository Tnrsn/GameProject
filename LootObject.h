// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Public/MasterItem.h"
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

	//Looting
	UPROPERTY(Blueprintreadwrite, EditAnywhere, Category = "Loot")
		TArray<FItemProperties> storage;
	UPROPERTY()
		UChildActorComponent* childActorRef;
	UPROPERTY(EditAnywhere, Category = "Loot")
		int lootLevet = 0;

	UPROPERTY()
		AMasterItem* item;

	UPROPERTY()
		APlayerController* playerController;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
		TSubclassOf<UUserWidget> LootUI;
	class UManageWidgets* HUD;

	bool lootUIEnabled = false;
	bool canOpenLoot = false;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintCallable)
	void EnableLootUI();


	void DisableLootUI(AActor* SelectedActor);

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(Blueprintcallable)
		void GenerateRandomItems(TArray<UClass*> itemsRef);
	UFUNCTION()
		void AddItemToLoot(FItemProperties itemProperties);


	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
