// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Components/SphereComponent.h>
#include "GameFramework/Actor.h"
#include "MasterItem.generated.h"

UENUM(BlueprintType)
enum FItemEnum
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
	//UPROPERTY(EditAnywhere, Blueprintreadwrite)
	//FItemEnum Category;
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
	UFUNCTION(BlueprintCallable)
		AMasterItem* ItemReference();
	UFUNCTION(BlueprintCallable)
		void DestroyObject();

	UPROPERTY(EditAnywhere, Blueprintreadwrite, Category = "Item Properties")
	FItemProperties ItemProperties;

	//UPROPERTY(Blueprintreadwrite, EditAnywhere)
	//	int test = 0;

	UPROPERTY(EditAnywhere)
	USphereComponent* SphereComponent;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};


