// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Public/ManageWidgets.h"
#include "LootObject.h"
#include "Public/MasterItem.h"
#include <Engine/World.h>
#include <Kismet/GameplayStatics.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/PlayerController.h>
#include "GameFramework/Character.h"
#include "PlayerControls.generated.h"

UCLASS()
class GAMEPROJECT_API APlayerControls : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerControls();

	UInputComponent* playerInput;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* Camera;

	UPROPERTY()
		AActor* SelectedActor;
	UPROPERTY(BlueprintReadonly)
		ALootObject* lootObject;

	APlayerController* playerController;

	//Widget Management
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
		TSubclassOf<UUserWidget> inventoryUI;
	class UManageWidgets* inventoryHUD;


	UPROPERTY(EditDefaultsOnly, Category = "Widget")
		TSubclassOf<UUserWidget> mainUI;
	UPROPERTY(BlueprintReadOnly)
	class UManageWidgets* mainHUD;

	//Looting
	UPROPERTY(Blueprintreadwrite)
		TArray<FItemProperties> inventory;
	UPROPERTY(BlueprintReadWrite)
		AMasterItem* itemRef;
	UPROPERTY(BlueprintReadWrite)
		bool itemTaken = false;

	UPROPERTY(BlueprintReadOnly)
		bool inventoryEnabled = false;

	//Camera
	bool camRotating = false;
	float targetArmLength = 250.f;
	float newTargetArmLength = 250.f;

	//PathFinding
	UPROPERTY(BlueprintReadWrite)
		bool onAIControl = true;
	UPROPERTY(BlueprintReadOnly)
		bool characterOnMove = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void StartCameraRotation();
	void StopCameraRotation();

	void MoveForward(float value);
	void MoveRight(float value);

	void CameraZoom(float value);
	void OnMouseClick();
	void ClickEvents();

	void OpenInventory();

	UFUNCTION(BlueprintCallable)
		void ItemInteraction(FItemProperties item);

	UFUNCTION()
	void AddItemToInventoryFromGround();
	UFUNCTION(BlueprintCallable)
		void ItemTransfer(FItemProperties itemProperties);
	UFUNCTION()
		bool AddItemToInventory(FItemProperties itemProperties);

	UFUNCTION()
		void ResetInventoryUI();
	UFUNCTION()
		void DecreaseItemFromInventory(FItemProperties itemProperties);
	UFUNCTION(BlueprintCallable)
		void PutOffItem(UManageWidgets* itemProperties, int WearableSlotIndex);
	UFUNCTION(BlueprintCallable)
		int GetArmorRating();


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

