// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Public/PlayerSpringArmComponent.h"
#include "Public/PlayerCameraComponent.h"
#include <Blueprint/AIBlueprintHelperLibrary.h>
#include <NavigationSystem.h>
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
		UPlayerSpringArmComponent* springArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		UPlayerCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UCapsuleComponent* capsuleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes")
		USkeletalMeshComponent* hairMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes")
		USkeletalMeshComponent* headMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes")
		USkeletalMeshComponent* handsMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes")
		USkeletalMeshComponent* footsMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes")
		USkeletalMeshComponent* torsoMesh;

	//Body Parts
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meshes")
		USkeletalMesh* hairBodyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meshes") //Male Meshes
		USkeletalMesh* headMaleBodyMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meshes")
		USkeletalMesh* handsMaleBodyMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meshes")
		USkeletalMesh* footsMaleBodyMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meshes")
		USkeletalMesh* torsoMaleBodyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meshes") //Female Meshes
		USkeletalMesh* headFemaleBodyMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meshes")
		USkeletalMesh* handsFemaleBodyMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meshes")
		USkeletalMesh* footsFemaleBodyMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meshes")
		USkeletalMesh* torsoFemaleBodyMesh;

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
	UPROPERTY()
		bool inMenu = true;

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

	UFUNCTION()
		void InitCharacter();
public:
	void StartCameraRotation();
	void StopCameraRotation();
	void CameraZoom(float value);

	UFUNCTION(BlueprintCallable)
		void MoveForward(float value);
	UFUNCTION(BlueprintCallable)
		void MoveRight(float value);

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
	UFUNCTION()
		FVector GetPlayerBehindLocation(float behind, float right);
	UFUNCTION()
		void ResetAnimations();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

