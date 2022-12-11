// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Components/SphereComponent.h>
#include "GameFramework/Actor.h"
#include "MasterItem.generated.h"

UENUM(BlueprintType)
enum FConsumableEffect
{
	Nothing,
	Heal,
	DamageHealth,
	BoostSpeed
};

UENUM(BlueprintType)
enum FItemCategory
{
	Weapon,
	Armor,
	Consumable
};


USTRUCT(BlueprintType)
struct FItemProperties
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, Blueprintreadwrite)
	FString name;
	UPROPERTY(EditAnywhere, Blueprintreadwrite)
	UTexture* texture;
	UPROPERTY(EditAnywhere, Blueprintreadwrite)
	int weight;
	UPROPERTY(EditAnywhere, Blueprintreadwrite)
	TEnumAsByte<FItemCategory> Category;
	UPROPERTY(EditAnywhere, Blueprintreadwrite)
		int maximumAmount;
	UPROPERTY(EditAnywhere, Blueprintreadwrite)
		bool isStackable;
	UPROPERTY(EditAnywhere, Blueprintreadwrite)
		FString description;
	UPROPERTY(EditAnywhere, Blueprintreadwrite)
		int currentAmount = 1;
	UPROPERTY(Blueprintreadonly, EditAnywhere)
		float rarity;
	UPROPERTY(EditAnywhere, Blueprintreadwrite)
		bool inInventory;
};

UCLASS()
class GAMEPROJECT_API AMasterItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMasterItem();

	UFUNCTION()
		void OnSelected(UPrimitiveComponent* PrimComp, FKey InKey);
	//UFUNCTION(BlueprintCallable)
	//	AMasterItem* ItemReference();
	//UFUNCTION(BlueprintCallable)
	//	void DestroyObject();

	UPROPERTY(EditAnywhere, Blueprintreadwrite, Category = "Item Properties")
	FItemProperties ItemProperties;

	UPROPERTY(EditAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere)
		class USphereComponent* TriggerCollision;

	bool canLoot = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	// Called every frame
	virtual void Tick(float DeltaTime) override;



};


