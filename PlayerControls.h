// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Particles/ParticleSystemComponent.h>
#include <Particles/ParticleSystem.h>
#include "Public/QuestSystem.h"
#include "DefaultGameMode.h"
#include "Engine/LevelStreaming.h"
#include "GroomComponent.h"
#include "SaveSystem.h"
#include "Public/PlayerSpringArmComponent.h"
#include "Public/PlayerCameraComponent.h"
#include <Blueprint/AIBlueprintHelperLibrary.h>
#include <NavigationSystem.h>
#include "Public/CharacterProfiles.h"
#include "Public/ManageWidgets.h"
#include "LootObject.h"
#include "Public/MasterItem.h"
#include <Kismet/GameplayStatics.h>
#include "Public/EnemyFinderComp.h"
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/PlayerController.h>
#include "GameFramework/Character.h"
#include "Public/DefaultGameInstance.h"
#include "Public/ClassSkills.h"
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
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//	UCapsuleComponent* capsuleComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UEnemyFinderComp* findEnemyComponent;

	//Skeletal Mesh Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes")
		UGroomComponent* hairGroom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes")
		USkeletalMeshComponent* headMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes")
		USkeletalMeshComponent* handsMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes")
		USkeletalMeshComponent* footsMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes")
		USkeletalMeshComponent* torsoMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes")
		UStaticMeshComponent* hand1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes")
		UStaticMeshComponent* hand2;

	//Body Parts - Male
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meshes")
		UGroomAsset* maleHairGroom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meshes") //Male Meshes
		USkeletalMesh* headMaleBodyMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meshes")
		USkeletalMesh* handsMaleBodyMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meshes")
		USkeletalMesh* footsMaleBodyMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meshes")
		USkeletalMesh* torsoMaleBodyMesh;

	//" " - Female
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meshes")
		UGroomAsset* femaleHairGroom;

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
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Widget")
		UManageWidgets* mainHUD;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
		TSubclassOf<UUserWidget> pauseUI;
	UPROPERTY()
		UManageWidgets* pauseHUD;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
		TSubclassOf<UUserWidget> deathUI;
	UPROPERTY()
		UManageWidgets* deathHUD;

	//Looting
	UPROPERTY(Blueprintreadwrite)
		TArray<FItemProperties> inventory;
	UPROPERTY(BlueprintReadWrite)
		AMasterItem* itemRef;
	UPROPERTY(BlueprintReadWrite)
		bool itemTaken = false;
	UPROPERTY(BlueprintReadOnly)
		bool inventoryEnabled = false;

	UPROPERTY(BlueprintReadWrite)
		bool firstArmorsEquipped = false;

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
	UPROPERTY()
		bool inCombat = false;
	UPROPERTY(BlueprintReadWrite)
		bool inDialog = false;
	UPROPERTY(BlueprintReadWrite)
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
	UPROPERTY()
		int controlledCharIndex;

	//SaveSystem
	UPROPERTY()
		USaveSystem* saveSystem;
	UPROPERTY()
		FString currentWorldName;
	static bool newLevelLoaded;
	static bool loadAfterNewWorld;

	bool firstEncounter = true;

	//Combat
	FTimerHandle pickEnemyTimer;

	float combatCounter = 0;
	float attackSpeed = .5f;

	//Combat Skills
	UPROPERTY(VisibleAnywhere)
		UClassSkills* skills;

	//Animation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimClasses")
		UClass* maleAnimRef;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimClasses")
		UGroomBindingAsset* maleHairBinding;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimClasses")
		UClass* femaleAnimRef;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimClasses")
		UGroomBindingAsset* femaleHairBinding;

	UPROPERTY(BlueprintReadOnly)
		bool punchAnim = false;
	UPROPERTY(BlueprintReadOnly)
		bool spellCasting = false;
	UPROPERTY(BlueprintReadOnly)
		bool slashAnim = false;
	//UPROPERTY(BlueprintReadOnly)
	//	bool dead = false;
	UPROPERTY(BlueprintReadOnly)
		bool defending = false;

	//Quest System
	UQuestSystem* questSystem;

	//VFX
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "VFX")
		UParticleSystem* sparks_F;

	//First Items
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "First Armor")
		TSubclassOf<AMasterItem> firstHeadGear;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "First Armor")
		TSubclassOf<AMasterItem> firstTopGear;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "First Armor")
		TSubclassOf<AMasterItem> firstHandsGear;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "First Armor")
		TSubclassOf<AMasterItem> firstBootsGear;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "First Armor")
		TSubclassOf<AMasterItem> firstHand1Gear;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "First Armor")
		TSubclassOf<AMasterItem> firstHand2Gear;

	//Pause Menu
	bool isPaused = false;
	int delayContinue = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION()
		void InitCharacter(UWorld* world);

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
	UFUNCTION()
		void PutOn2FirstHand(FItemProperties itemProperties, bool DecreaseFromInventory = true);
	UFUNCTION()
		void PutOn2SecondHand(FItemProperties itemProperties, bool DecreaseFromInventory = true);
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
	//AI movement
	UFUNCTION()
		void StopAIOnInput(); //Checks if player interrupted to ai movement
	UFUNCTION()
		void StopAIMovement(bool goalDone); //Stops every ai movement
	//Group functions
	UFUNCTION()
		void SmoothCameraSwitch(int index, float moveSpeed = 1.f);
	UFUNCTION()
		void FollowControlledCharacter();
	UFUNCTION()
		FVector GetPlayerBehindLocation(float behind, float right);
	UFUNCTION()
		void ResetAnimations();
	//Save load
	UFUNCTION()
		void SaveGame();
	UFUNCTION(BlueprintCallable)
		void LoadGame();
	//Combat
	UFUNCTION()
		void Attack(float DeltaTime, AActor* enemyActor);
	UFUNCTION()
		int CalculateDamage(AActor* enemyActor);
	UFUNCTION()
		bool isDamagedEnemy(AActor* enemyActor);
	UFUNCTION()
		void NPCInteractions(float DeltaTime);
	UFUNCTION()
		void TurnToEnemy(FVector enemyLocation);
	UFUNCTION()
		void StartCombat(AActor* enemy);
	UFUNCTION()
		void ApplyDamage(int damage);
	UFUNCTION()
		void HitFast();

	UFUNCTION()
		bool isGroupDead();
	UFUNCTION()
		void CheckCombatAnim(AActor* enemyActor);
	UFUNCTION()
		void RefillHealthAndEnergy();

	//Combat AI Behaviour
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
		void OverlappedWithActor(AActor* OtherActor);
	//Combat Skills
	UFUNCTION()
		void SkillOne();
	UFUNCTION()
		void SkillTwo();
	UFUNCTION()
		void SkillThree();
	UFUNCTION()
		void CancelSkill();
	//Slowing time
	UFUNCTION()
		void SlowTime();
	UFUNCTION()
		void BackToNormalTime();
	//Set Appearence
	UFUNCTION(BlueprintCallable)
		void SetMesh();
	UFUNCTION()
		void SetFirstItems();
	UFUNCTION()
		void PutOnItem(TSubclassOf<AMasterItem> itemClass);

	//Pause
	UFUNCTION(BlueprintCallable)
		void PauseGame();
	UFUNCTION()
		void WhenPaused();
	UFUNCTION()
		bool IsInMenuLevel() const;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

bool APlayerControls::newLevelLoaded = false; //It's for when player loads save
bool APlayerControls::loadAfterNewWorld = false; //It's for when character transports between worlds