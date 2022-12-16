// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Components/SphereComponent.h>
#include "GameFramework/Actor.h"
#include "MasterItem.generated.h"

UENUM(BlueprintType, Category = "Item Properties")
enum FConsumableEffect
{
	NotConsumable,
	Heal,
	DamageHealth,
	BoostSpeed
};
UENUM(BlueprintType, Category = "Item Properties")
enum FArmorType
{
	NotArmor,
	Head,
	Top,
	Hand,
	Foot,
	FirstRing,
	SecondRing,
	Neck
};

UENUM(BlueprintType, Category = "Item Properties")
enum FItemCategory
{
	Weapon,
	Armor,
	Consumable
};


USTRUCT(BlueprintType, Category = "Item Properties")
struct FItemProperties
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, Blueprintreadwrite)
		FString name;
	UPROPERTY(EditAnywhere, Blueprintreadwrite)
		FString description;
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
	UPROPERTY(Blueprintreadwrite)
		int currentAmount = 1;
	UPROPERTY(Blueprintreadonly, EditAnywhere)
		float rarity;
	UPROPERTY(Blueprintreadwrite)
		bool inInventory;
	//Consumable Inputs
	UPROPERTY(EditAnywhere, Blueprintreadwrite)
		TEnumAsByte<FConsumableEffect> ConsumableEffect;
	UPROPERTY(EditAnywhere)
		int effectStrength;
	UPROPERTY(EditAnywhere)
		int effectTime;
	//Armor
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TEnumAsByte<FArmorType> ArmorType;
	UPROPERTY(BlueprintReadWrite)
		bool isEquipped = false;
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

	UPROPERTY(EditAnywhere, Blueprintreadwrite, Category = "Item Properties")
	FItemProperties ItemProperties;

	UPROPERTY(EditAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere)
		class USphereComponent* TriggerCollision;

	bool canLoot = false;

	UPROPERTY(BlueprintReadWrite)
		bool moveToObject = false;

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


