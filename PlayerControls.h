// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Public/CharacterAIController.h"
#include <Blueprint/AIBlueprintHelperLibrary.h>
#include <NavigationSystem.h>
//#include "Public/PathFindingSystem.h"
#include "Public/CharacterProfiles.h"
#include "Public/ManageWidgets.h"
#include "LootObject.h"
#include "Public/MasterItem.h"
//#include <Engine/World.h>
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

	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UCapsuleComponent* capsuleComp;

	//Camera System
	FVector2D mousePosition;
	FVector2D mouseDelta;
	FVector2D previousMousePosition;

	float mouseSensitivity = .3f;
	bool rotationStarted = false;

	//ClickEvent Objects
	UPROPERTY(BlueprintReadWrite)
		AActor* SelectedActor;
	UPROPERTY(BlueprintReadonly)
		ALootObject* lootObject;

	//Player controller
	UPROPERTY()
		APlayerController* playerController;
	UPROPERTY()
		UInputComponent* playerInput;

	//Widget Management
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
		TSubclassOf<UUserWidget> inventoryUI;
	UPROPERTY()
		UManageWidgets* inventoryHUD;
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
		TSubclassOf<UUserWidget> mainUI;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		UManageWidgets* mainHUD;

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
	UPROPERTY()
		bool camRotating = false;
	UPROPERTY()
		float newTargetArmLength = 250.f;

	//PathFinding
	UPROPERTY()
		FVector targetLocation;
	UPROPERTY()
		AActor* actorToBeGone;

	//Character interactions
	UPROPERTY(BlueprintReadWrite)
		bool inDialog = false;
	UPROPERTY(BlueprintReadOnly)
		UCharacterProfiles* characterProfile;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<APlayerControls*> groupMembers;
	UPROPERTY()
		bool inGroup = false;

	//AI Controls
	UPROPERTY()
		bool onAIMovement = false;
	UPROPERTY()
		bool followingChar;
	UPROPERTY()
		bool onAIControl = true;
	UPROPERTY()
		APlayerControls* controlledChar;
	UPROPERTY()
		int charIndex;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void StartCameraRotation();
	void StopCameraRotation();
	void RotateCamera();

	UFUNCTION(BlueprintCallable)
		void MoveForward(float value);
	UFUNCTION(BlueprintCallable)
		void MoveRight(float value);

	void CameraZoom(float value);
	void OnMouseClick();
	void ClickEvents();

	void ToggleInventory();

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
		void PutOffItem(UCharacterProfiles* itemProperties, int WearableSlotIndex);
	UFUNCTION(BlueprintCallable)
		int GetArmorRating();

	UFUNCTION()
		void ControlFirstCharacter();
	UFUNCTION()
		void ControlSecondCharacter();
	UFUNCTION()
		void ControlThirdCharacter();
	UFUNCTION()
		void ControlFourthCharacter();
	UFUNCTION()
		void ControlNPC(int index);
	UFUNCTION()
		void MoveToLocation(const AActor* actor, const FVector& Location);
	UFUNCTION()
		bool CheckIfAnyUIEnabled();
	UFUNCTION()
		void StopAIMovement(bool goalDone);
	UFUNCTION()
		void SmoothCameraSwitch(int index, float moveSpeed = 1.f);
	UFUNCTION()
		void FollowControlledCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

