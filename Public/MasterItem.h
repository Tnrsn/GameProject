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
enum FWearableType
{
	NotWearable,
	Head,
	Top,
	Hand,
	Foot,
	FirstRing,
	SecondRing,
	Neck,
	TwoHandedWeapon,
	OneHandedWeapon,
	Shield
};

UENUM(BlueprintType, Category = "Item Properties")
enum FItemCategory
{
	Weapon,
	Armor,
	Consumable
};

UENUM(BlueprintType, Category = "Item Properties")
enum FWeaponType
{
	Melee,
	Ranged
};

UENUM(BlueprintType, Category = "Item Properties")
enum FDamageType
{
	StrDamage,
	IntDamage
};

USTRUCT(BlueprintType, Category = "Item Properties")
struct FItemProperties
{
	GENERATED_BODY();
	FItemProperties()
		: name("")
		, description("")
		, weight(0)
		, Category(0)
		, maximumAmount(0)
		, isStackable(0)
		, currentAmount(0)
		, rarity(0)
		, inInventory(0)
		, ConsumableEffect(0)
		, effectStrength(0)
		, effectTime(0)
		, WearableType(0)
		, armorBonus(0)
		, isEquipped(0)
		, weapon2Item(0)
		, damageBonus(0)
		, location(0)
		, rotation(0)
		, scale(0)
		, texturePath("")
		, skeletalMeshPath_M("")
		, skeletalMeshPath_F("")
		, staticMeshPath("")
	{}

	UPROPERTY(EditAnywhere, Blueprintreadwrite)
		FString name;
	UPROPERTY(EditAnywhere, Blueprintreadwrite)
		FString description;
	UPROPERTY(EditAnywhere, Blueprintreadwrite)
		UTexture* texture = nullptr;
	UPROPERTY(EditAnywhere, Blueprintreadwrite)
		int weight;
	UPROPERTY(EditAnywhere, Blueprintreadwrite)
		TEnumAsByte<FItemCategory> Category;
	UPROPERTY(EditAnywhere, Blueprintreadwrite)
		int maximumAmount;
	UPROPERTY(EditAnywhere, Blueprintreadwrite)
		bool isStackable;
	UPROPERTY(Blueprintreadwrite, EditAnyWhere)
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
	//Wearables
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TEnumAsByte<FWearableType> WearableType;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int armorBonus;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool isEquipped = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USkeletalMesh* skeletalMesh_M = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USkeletalMesh* skeletalMesh_F = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UStaticMesh* staticMesh = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector location = FVector(0);
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FQuat rotation = FQuat(0);
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector scale = FVector(0);
	UPROPERTY(BlueprintReadOnly)
		bool hideHeadMesh = false;
	UPROPERTY(BlueprintReadOnly)
		bool weapon2Item = false;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int damageBonus;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TEnumAsByte<FWeaponType> weaponType;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TEnumAsByte<FDamageType> damageType;

	//Pointer paths for saving
	UPROPERTY() //texturePath and SkeletalMeshPath are assigning while saving game in SaveSystem.h
		FString texturePath;
	UPROPERTY()
		FString skeletalMeshPath_M;
	UPROPERTY()
		FString skeletalMeshPath_F;
	UPROPERTY()
		FString staticMeshPath;
};
//Everytime I add a new variable to FItemProperties dont forget to add it to SaveSystem.h SaveItem function too.

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


