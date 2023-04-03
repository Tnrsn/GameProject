// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControls.h"
#include "NPC_Management.h"

// Sets default values
APlayerControls::APlayerControls()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	springArm = CreateDefaultSubobject<UPlayerSpringArmComponent>("springArm");
	springArm->SetupAttachment(RootComponent);
	springArm->TargetArmLength = 250;
	springArm->bUsePawnControlRotation = false;

	Camera = CreateDefaultSubobject<UPlayerCameraComponent>("Camera");
	Camera->SetupAttachment(springArm, UPlayerSpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	findEnemyComponent = CreateDefaultSubobject<UEnemyFinderComp>(TEXT("Find Enemy Component"));
	findEnemyComponent->SetupAttachment(RootComponent);
	
	//Meshes
	headMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Head Mesh");
	headMesh->SetupAttachment(RootComponent);

	handsMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Hand Mesh");
	handsMesh->SetupAttachment(RootComponent);

	footsMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Foot Mesh");
	footsMesh->SetupAttachment(RootComponent);

	torsoMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Torso Mesh");
	torsoMesh->SetupAttachment(RootComponent);

	hairGroom = CreateDefaultSubobject<UGroomComponent>("Hair Groom");
	hairGroom->SetupAttachment(headMesh);
	//****
}

// Called when the game starts or when spawned
void APlayerControls::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		saveSystem = GetWorld()->GetSubsystem<USaveSystem>();
	}
	
	playerController = UGameplayStatics::GetPlayerController(this, 0);
	characterProfile = NewObject<UCharacterProfiles>();
	skills = NewObject<UClassSkills>();

	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
	GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;
	GetWorld()->GetFirstPlayerController()->bEnableMouseOverEvents = true;

	if (*GetWorld()->GetName() != FName("MainMenu") && *GetWorld()->GetName() != FName("CharacterCreationMenu"))
	{//If player is in game then enables game inputs and initializes game character
		FInputModeGameAndUI inputMode;
		inputMode.SetHideCursorDuringCapture(false);
		playerController->SetInputMode(inputMode);

		inMenu = false;
		InitCharacter();
	}
	else
	{//If player is in menu or character creation menu then disables every input except ui inputs
		FInputModeUIOnly inputMode;
		inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		playerController->SetInputMode(inputMode);

		springArm->LookFromFront();
	}
}

// Called every frame
void APlayerControls::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (camRotating)
	{
		springArm->RotateCamera(playerController);
	}

	if (itemRef) //Interacts with items around world
	{
		if (itemRef->canLoot && itemRef->moveToObject)
		{
			AddItemToInventoryFromGround();
		}
	}
	//if (questSystem)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("%s"), (questSystem->mainQuestLine[1] ? TEXT("true") : TEXT("false")));
	//}

	if (!dead)
	{
		//Interactions with npcs (talking or combat)
		NPCInteractions(DeltaTime);

		//Stops ai movement If Its necessary
		StopAIOnInput();

		//Character dies if their health is under of 0
		if (*GetWorld()->GetName() != FName("MainMenu") && *GetWorld()->GetName() != FName("CharacterCreationMenu") && characterProfile->characterCurrentHealth <= 0)
		{
			StopAIMovement(true);
			dead = true;
			//Destroy();
		}

		//Npcs in group follows controlled character
		FollowControlledCharacter();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Dead"), *GetName());
	}

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *StaticEnum<FCharacterClasses>()->GetValueAsString(characterProfile->charClass));
}

void APlayerControls::InitCharacter()
{
	mainHUD = CreateWidget<UManageWidgets>(UGameplayStatics::GetPlayerController(GetWorld(), 0), mainUI);
	currentWorldName = GetWorld()->GetName();
	
	if (groupMembers.Num() == 0 && GetController() == playerController)
	{
		groupMembers.Add(Cast<APlayerControls>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)));
		inGroup = true;
		onAIControl = false;
		charIndex = 0;

		mainHUD->characterProfiles = NewObject<UCharacterProfiles>();

		mainHUD->characterProfiles = characterProfile;
		mainHUD->AddToViewport();

		characterProfile->RefreshStats();
		characterProfile->characterCurrentHealth = characterProfile->characterMaximumHealth;
		characterProfile->characterCurrentEnergy = characterProfile->characterMaximumEnergy;

		questSystem = NewObject<UQuestSystem>();
	}

	findEnemyComponent->OnComponentBeginOverlap.Clear();
	findEnemyComponent->OnComponentEndOverlap.Clear();
	findEnemyComponent->OnComponentBeginOverlap.AddDynamic(this, &APlayerControls::OnOverlapBegin);
	findEnemyComponent->OnComponentEndOverlap.AddDynamic(this, &APlayerControls::OnOverlapEnd);


	if (inGroup)
	{
		controlledChar = Cast<APlayerControls>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}
	
	if (APlayerControls::newLevelLoaded && *GetClass()->GetSuperClass()->GetName() == FString("PlayerControls"))
	{
		LoadGame();
		APlayerControls::newLevelLoaded = false;
	}

	if (APlayerControls::toNewWorld && *GetClass()->GetSuperClass()->GetName() == FString("PlayerControls"))
	{
		saveSystem->LoadGame("TransportSave", true);
	}
}

// Called to bind functionality to input
void APlayerControls::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	playerInput = PlayerInputComponent;


	PlayerInputComponent->BindAxis("MoveForward / Backward", this, &APlayerControls::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &APlayerControls::MoveRight);

	//Camera inputs
	PlayerInputComponent->BindAction("RightClick", IE_Pressed, this, &APlayerControls::StartCameraRotation);
	PlayerInputComponent->BindAction("RightClick", IE_Released, this, &APlayerControls::StopCameraRotation);
	PlayerInputComponent->BindAxis("CameraZoom", this, &APlayerControls::CameraZoom);

	PlayerInputComponent->BindAction("LeftClick", IE_Pressed, this, &APlayerControls::OnMouseClick);
	PlayerInputComponent->BindAction("ToggleInventory", IE_Pressed, this, &APlayerControls::ToggleInventory);

	//Character switch inputs
	PlayerInputComponent->BindAction("PassToFirstCharacter", IE_Pressed, this, &APlayerControls::ControlFirstCharacter);
	PlayerInputComponent->BindAction("PassToSecondCharacter", IE_Pressed, this, &APlayerControls::ControlSecondCharacter);
	PlayerInputComponent->BindAction("PassToThirdCharacter", IE_Pressed, this, &APlayerControls::ControlThirdCharacter);
	PlayerInputComponent->BindAction("PassToFourthCharacter", IE_Pressed, this, &APlayerControls::ControlFourthCharacter);

	//Save/Load inputs
	PlayerInputComponent->BindAction("QuickSave", IE_Pressed, this, &APlayerControls::SaveGame);
	PlayerInputComponent->BindAction("QuickLoad", IE_Pressed, this, &APlayerControls::LoadGame);

	//Skill inputs
	PlayerInputComponent->BindAction("SkillOne", IE_Pressed, this, &APlayerControls::SkillOne);
	PlayerInputComponent->BindAction("SkillTwo", IE_Pressed, this, &APlayerControls::SkillTwo);
	PlayerInputComponent->BindAction("SkillThree", IE_Pressed, this, &APlayerControls::SkillThree);

	//Time inputs
	PlayerInputComponent->BindAction("SlowTime", IE_Pressed, this, &APlayerControls::SlowTime);
	PlayerInputComponent->BindAction("SlowTime", IE_Released, this, &APlayerControls::BackToNormalTime);

	PlayerInputComponent->BindAction("FastHit", IE_Pressed, this, &APlayerControls::HitFast);
}

void APlayerControls::OverlappedWithActor(AActor* OtherActor)
{
	if (OtherActor && OtherActor != this && Cast<ACharacter>(OtherActor))
	{
		if (Cast<ANPC_Management>(OtherActor))
		{
			ANPC_Management* enemy = Cast<ANPC_Management>(OtherActor);
			if (Cast<ANPC_Management>(this)) //If this is not main character...
			{
				ANPC_Management* ally = Cast<ANPC_Management>(this);

				//If npc is not in group or if npc is not a Hostile then npc wont attack anyone
				//To attack anyone npc has to be in group or has to be a hostile
				if (ally->NPCStyle != Hostile && !ally->inGroup) return;

				if (ally->NPCStyle == Hostile)
				{
					//Hostile Npc only attacks to npcs in group
					if (onAIControl && enemy->inGroup)
					{
						StartCombat(enemy);
					}
				}
				else
				{
					if (onAIControl && enemy->NPCStyle == Hostile)
					{
						StartCombat(enemy);
					}
				}
			}
			else
			{
				//Main characters interaction with npcs
				//If main character is on AI control then attacks to enemy
				if (enemy->NPCStyle == Hostile)
				{
					StartCombat(enemy);
				}

			}
		}
		else
		{
			//If npc overlaps with main character
			if (Cast<APlayerControls>(OtherActor) && Cast<ANPC_Management>(this))
			{
				ANPC_Management* npc = Cast<ANPC_Management>(this);
				if (onAIControl && npc->NPCStyle == Hostile)
				{
					StartCombat(OtherActor);
				}
			}
		}
	}
}

void APlayerControls::MoveForward(float value)
{
	if ((Controller != nullptr) && (value != 0) && (!inDialog) && !dead)
	{
		if (lootObject)
		{
			lootObject->moveToLootObject = false;
			lootObject->DisableLootUI(SelectedActor);
		}
		if (itemRef)
		{
			itemRef->moveToObject = false;
		}
		if (inventoryEnabled)
		{
			ToggleInventory();
		}
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Camera->GetComponentRotation().Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);
	}
}

void APlayerControls::MoveRight(float value)
{
	if ((Controller != nullptr) && (value != 0) && (!inDialog) && !dead)
	{
		if (lootObject)
		{
			lootObject->moveToLootObject = false;
			lootObject->DisableLootUI(SelectedActor);
		}
		if (itemRef)
		{
			itemRef->moveToObject = false;
		}
		if (inventoryEnabled)
		{
			ToggleInventory();
		}

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Camera->GetComponentRotation().Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, value);
	}
}

void APlayerControls::StartCameraRotation()
{
	if (lootObject)
	{
		if (!lootObject->lootUIEnabled && !inventoryEnabled)
		{
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
			GetWorld()->GetFirstPlayerController()->bEnableClickEvents = false;
			GetWorld()->GetFirstPlayerController()->bEnableMouseOverEvents = false;
			camRotating = true;
			springArm->rotationStarted = true;
		}
	}
	else if (!inventoryEnabled)
	{
		GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
		GetWorld()->GetFirstPlayerController()->bEnableClickEvents = false;
		GetWorld()->GetFirstPlayerController()->bEnableMouseOverEvents = false;
		camRotating = true;
		springArm->rotationStarted = true;
	}
}

void APlayerControls::StopCameraRotation()
{
	if (camRotating)
	{
		GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
		GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;
		GetWorld()->GetFirstPlayerController()->bEnableMouseOverEvents = true;
		camRotating = false;
	}
}

void APlayerControls::CameraZoom(float value)
{
	if (!inMenu && !inventoryEnabled && !inDialog)
	{
		newTargetArmLength -= value * 100;
		newTargetArmLength = FMath::Clamp(newTargetArmLength, 150.f, 1200.f);
		springArm->TargetArmLength = FMath::FInterpTo(springArm->TargetArmLength, newTargetArmLength, GetWorld()->GetDeltaSeconds(), 5.f);
	}
}

void APlayerControls::OnMouseClick()
{
	if (!inDialog)
	{
		if (lootObject)
		{
			if (!lootObject->lootUIEnabled && !inventoryEnabled)
			{
				ClickEvents();
			}
		}
		else
		{
			if (!inventoryEnabled)
			{
				ClickEvents();
			}
		}
	}
}

void APlayerControls::ClickEvents()
{
	FHitResult HitResult;
	playerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, HitResult);

	if (lootObject)
	{
		lootObject->moveToLootObject = false;
		lootObject->DisableLootUI(SelectedActor);
	}

	if (itemRef)
	{
		itemRef->moveToObject = false;
	}

	SelectedActor = HitResult.GetActor();

	if (SelectedActor)
	{
		if (skills->skillOneTargeting) //Activating Skills
		{
			StopAIMovement(true);
			skills->SkillOne(characterProfile->charClass, this, HitResult.Location);
		}
		else if (skills->skillTwoTargeting)
		{
			StopAIMovement(true);
			skills->SkillTwo(characterProfile->charClass, this, HitResult.Location);
		}
		else if (skills->skillThreeTargeting)
		{
			StopAIMovement(true);
			skills->SkillThree(characterProfile->charClass, this, HitResult.Location);
		}
		//Moving to objects
		else if (*SelectedActor->GetClass()->GetSuperClass()->GetName() == FName("BP_LootObject_C"))//Open Loot
		{
			lootObject = Cast<ALootObject>(SelectedActor);
			lootObject->EnableLootUI();

			actorToBeGone = SelectedActor;
			MoveToLocation(lootObject, FVector(0));
			inCombat = false;
		}
		else if (*SelectedActor->GetClass()->GetSuperClass()->GetName() == FName("BP_MasterItem_C"))//itemRef
		{
			itemRef = Cast<AMasterItem>(SelectedActor);
			AddItemToInventoryFromGround();

			actorToBeGone = SelectedActor;
			MoveToLocation(itemRef, FVector(0));
			inCombat = false;
		}
		//Interactions with npcs
		else if (*SelectedActor->GetClass()->GetSuperClass()->GetName() == FName("BP_NPC_Management_C") && SelectedActor != this) 
		{
			ANPC_Management* npc = Cast<ANPC_Management>(SelectedActor);
			if (npc->NPCStyle == NonTalkable)
			{
				return;
			}
			else if (npc->NPCStyle == Talkable) //Starting a dialog with npcs
			{ 
				inCombat = false;
				if (GetDistanceTo(npc) < 250)
				{
					//if there are enemies nearby, player wont able to talk with npcs
					if (findEnemyComponent->nearbyEnemies.Num() > 0) return;

					inDialog = true;
					npc->StartDialog();

					StopAIMovement(true);
					SetActorRotation((npc->GetActorLocation() - GetActorLocation()).Rotation());
				}
				else //If npc is far then character moves to npc
				{
					actorToBeGone = SelectedActor;
					MoveToLocation(actorToBeGone, FVector(0));
				}
			}
			else if (npc->NPCStyle == Hostile)
			{
				actorToBeGone = SelectedActor;
				inCombat = true;
			}
		}
		else
		{
			inCombat = false;
			targetLocation = HitResult.Location;
			actorToBeGone = nullptr;
			MoveToLocation(nullptr, targetLocation);
		}
	}

	//MoveToLocation(HitResult.Location);
}

void APlayerControls::ToggleInventory()
{
	if (lootObject)
	{
		if (!lootObject->lootUIEnabled)
		{
			if (!inventoryEnabled)
			{
				inventoryHUD = CreateWidget<UManageWidgets>(UGameplayStatics::GetPlayerController(GetWorld(), 0), inventoryUI);
				inventoryHUD->AddToViewport();

				inventoryEnabled = true;
			}
			else if (inventoryEnabled)
			{
				inventoryHUD->RemoveFromParent();

				inventoryEnabled = false;
			}
		}
		else
		{
			lootObject->DisableLootUI(SelectedActor);
		}
	}
	else
	{
		if (!inventoryEnabled)
		{
			inventoryHUD = CreateWidget<UManageWidgets>(UGameplayStatics::GetPlayerController(GetWorld(), 0), inventoryUI);
			inventoryHUD->AddToViewport();

			inventoryEnabled = true;
		}
		else if (inventoryEnabled)
		{
			inventoryHUD->RemoveFromParent();

			inventoryEnabled = false;
		}
	}
}

void APlayerControls::AddItemToInventoryFromGround()
{
	if (itemRef->canLoot)
	{
		bool taken = AddItemToInventory(itemRef->ItemProperties);
		itemTaken = true;
		
		if (taken)
		{
			itemRef->Destroy();
		}
	}
	else
	{
		itemRef->moveToObject = true;
	}
}

void APlayerControls::ItemTransfer(FItemProperties itemProperties)
{
	if (!inventoryEnabled)
	{
		int itemIndex = 0;
		int lastItemIndex = 0;

		if (itemProperties.inInventory)
		{
			//inventory to loot
			lootObject->AddItemToLoot(itemProperties);
			DecreaseItemFromInventory(itemProperties);
		}
		else
		{
			//loot to inventory
			bool taken = AddItemToInventory(itemProperties);

			if (taken)
			{
				for (FItemProperties el : lootObject->storage)
				{
					if (el.name == itemProperties.name)
					{
						lastItemIndex = itemIndex;
					}
					itemIndex++;
				}

				if (lootObject->storage[lastItemIndex].currentAmount > 1)
				{
					lootObject->storage[lastItemIndex].currentAmount--;
				}
				else
				{
					lootObject->storage.RemoveAt(lastItemIndex);
				}
			}
		}
		lootObject->HUD->RemoveFromParent();
		lootObject->HUD = CreateWidget<UManageWidgets>(UGameplayStatics::GetPlayerController(GetWorld(), 0), lootObject->LootUI);
		lootObject->HUD->AddToViewport();
	}

}

bool APlayerControls::AddItemToInventory(FItemProperties itemProperties)
{
	
	
	if (characterProfile->currentInventoryWeight + itemProperties.weight <= characterProfile->maxInventoryCapacity)
	{
		bool added = false;

		for (FItemProperties& var : inventory)
		{
			if (itemProperties.name == var.name)
			{
				if (itemProperties.isStackable && var.currentAmount < var.maximumAmount)
				{
					var.currentAmount++;
					added = true;
				}
			}
		}
		if (!added)
		{
			itemProperties.inInventory = true;
			itemProperties.currentAmount = 1;
			inventory.Add(itemProperties);
		}
		characterProfile->currentInventoryWeight += itemProperties.weight;
		return true;
	}
	else
	{
		return false;
	}
}

void APlayerControls::ResetInventoryUI()
{
	inventoryHUD->RemoveFromParent();
	inventoryHUD = CreateWidget<UManageWidgets>(UGameplayStatics::GetPlayerController(GetWorld(), 0), inventoryUI);
	inventoryHUD->AddToViewport();
}

void APlayerControls::DecreaseItemFromInventory(FItemProperties itemProperties)
{
	int lastItemIndex = 0;
	int itemIndex = 0;

	for (FItemProperties el : inventory)
	{
		if (el.name == itemProperties.name)
		{
			lastItemIndex = itemIndex;
		}
		itemIndex++;
	}

	if (inventory[lastItemIndex].currentAmount > 1)
	{
		inventory[lastItemIndex].currentAmount--;
	}
	else
	{
		inventory.RemoveAt(lastItemIndex);
	}

	characterProfile->currentInventoryWeight -= itemProperties.weight;
}

void APlayerControls::PutOffItem(UCharacterProfiles* itemProperties, int WearableSlotIndex)
{
	if (WearableSlotIndex + 1 == Head && itemProperties->characterArmor.head.isEquipped) //Head (This ranking is determined within AddWearableSlots in WP_Inventory Blueprint.)
	{
		//headMesh->SetVisibility(true);
		//if (HairGroomAsset)
		//{
		//	hairGroom->SetGroomAsset(HairGroomAsset);
		//}
		//else
		//{
		//	hairGroom->SetGroomAsset(NULL);
		//}

		characterProfile->currentInventoryWeight -= itemProperties->characterArmor.head.weight;
		AddItemToInventory(itemProperties->characterArmor.head);
		itemProperties->characterArmor.head = FItemProperties();
	}
	else if (WearableSlotIndex + 1 == Top && itemProperties->characterArmor.top.isEquipped) //Top
	{
		if (characterProfile->charGender == Male)
		{
			torsoMesh->SetSkeletalMesh(torsoMaleBodyMesh);
		}
		else
		{
			torsoMesh->SetSkeletalMesh(torsoFemaleBodyMesh);
		}

		characterProfile->currentInventoryWeight -= itemProperties->characterArmor.top.weight;
		AddItemToInventory(itemProperties->characterArmor.top);
		itemProperties->characterArmor.top = FItemProperties();
	}
	else if (WearableSlotIndex + 1 == Hand && itemProperties->characterArmor.hand.isEquipped) //Hand
	{
		if (characterProfile->charGender == Male)
		{
			handsMesh->SetSkeletalMesh(handsMaleBodyMesh);
		}
		else
		{
			handsMesh->SetSkeletalMesh(handsFemaleBodyMesh);
		}

		characterProfile->currentInventoryWeight -= itemProperties->characterArmor.hand.weight;
		AddItemToInventory(itemProperties->characterArmor.hand);
		itemProperties->characterArmor.hand = FItemProperties();
	}
	else if (WearableSlotIndex + 1 == Foot && itemProperties->characterArmor.foot.isEquipped) //Foot
	{
		if (characterProfile->charGender == Male)
		{
			footsMesh->SetSkeletalMesh(footsMaleBodyMesh);
		}
		else
		{
			footsMesh->SetSkeletalMesh(footsFemaleBodyMesh);
		}

		characterProfile->currentInventoryWeight -= itemProperties->characterArmor.foot.weight;
		AddItemToInventory(itemProperties->characterArmor.foot);
		itemProperties->characterArmor.foot = FItemProperties();
	}
	else if (WearableSlotIndex + 1 == FirstRing && itemProperties->characterArmor.firstRing.isEquipped) //Ring1
	{
		characterProfile->currentInventoryWeight -= itemProperties->characterArmor.firstRing.weight;
		AddItemToInventory(itemProperties->characterArmor.firstRing);
		itemProperties->characterArmor.firstRing = FItemProperties();
	}
	else if (WearableSlotIndex + 1 == SecondRing && itemProperties->characterArmor.secondRing.isEquipped) //Ring2
	{
		characterProfile->currentInventoryWeight -= itemProperties->characterArmor.secondRing.weight;
		AddItemToInventory(itemProperties->characterArmor.secondRing);
		itemProperties->characterArmor.secondRing = FItemProperties();
	}
	else if (WearableSlotIndex + 1 == Neck && itemProperties->characterArmor.neck.isEquipped) //Neck
	{
		characterProfile->currentInventoryWeight -= itemProperties->characterArmor.neck.weight;
		AddItemToInventory(itemProperties->characterArmor.neck);
		itemProperties->characterArmor.neck = FItemProperties();
	}
	else if (WearableSlotIndex == 7 && itemProperties->characterArmor.weapon1.isEquipped)
	{
		characterProfile->currentInventoryWeight -= itemProperties->characterArmor.weapon1.weight;
		AddItemToInventory(itemProperties->characterArmor.weapon1);
		itemProperties->characterArmor.weapon1 = FItemProperties();

		if (itemProperties->characterArmor.weapon2.isEquipped && itemProperties->characterArmor.weapon2.WearableType != Shield)
		{
			itemProperties->characterArmor.weapon1 = itemProperties->characterArmor.weapon2;
			itemProperties->characterArmor.weapon2 = FItemProperties();
		}
	}
	else if (WearableSlotIndex == 8 && itemProperties->characterArmor.weapon2.isEquipped)
	{
		characterProfile->currentInventoryWeight -= itemProperties->characterArmor.weapon2.weight;
		AddItemToInventory(itemProperties->characterArmor.weapon2);
		itemProperties->characterArmor.weapon2 = FItemProperties();
	}
	ResetInventoryUI();
	ResetAnimations();
}

int APlayerControls::GetArmorRating()
{
	int totalArmorRating = 0;
	totalArmorRating += characterProfile->characterArmor.head.armorBonus;
	totalArmorRating += characterProfile->characterArmor.top.armorBonus;
	totalArmorRating += characterProfile->characterArmor.hand.armorBonus;
	totalArmorRating += characterProfile->characterArmor.neck.armorBonus;
	totalArmorRating += characterProfile->characterArmor.firstRing.armorBonus;
	totalArmorRating += characterProfile->characterArmor.secondRing.armorBonus;
	totalArmorRating += characterProfile->characterArmor.foot.armorBonus;
	totalArmorRating += characterProfile->characterArmor.weapon1.armorBonus;
	totalArmorRating += characterProfile->characterArmor.weapon2.armorBonus;
	return totalArmorRating;
}

void APlayerControls::ItemInteraction(FItemProperties itemProperties) //Called in Item Slot blueprint --- Equips or consumes the items in this function
{
	// Weapon *******************************************************************
	if (itemProperties.Category == Weapon) 
	{
		if (itemProperties.WearableType == OneHandedWeapon)
		{
			if (characterProfile->characterArmor.weapon1.isEquipped)
			{
				if (characterProfile->characterArmor.weapon1.WearableType == TwoHandedWeapon)
				{
					PutOffItem(characterProfile, 7);
				}
				else if (characterProfile->characterArmor.weapon2.isEquipped)
				{
					if (characterProfile->characterArmor.weapon2.WearableType == Shield) //If weapon2 slots has a onehandedwaepon then put off it and wear new selected wearable
					{
						PutOffItem(characterProfile, 7);
						characterProfile->characterArmor.weapon1 = itemProperties;
						characterProfile->characterArmor.weapon1.isEquipped = true;
						DecreaseItemFromInventory(characterProfile->characterArmor.weapon1);
						characterProfile->currentInventoryWeight += characterProfile->characterArmor.weapon1.weight;
					}
					else if(characterProfile->characterArmor.weapon2.WearableType == OneHandedWeapon)
					{
						PutOffItem(characterProfile, 8);
						characterProfile->characterArmor.weapon2 = itemProperties;
						characterProfile->characterArmor.weapon2.isEquipped = true;
						characterProfile->characterArmor.weapon2.weapon2Item = true;
						DecreaseItemFromInventory(characterProfile->characterArmor.weapon2);
						characterProfile->currentInventoryWeight += characterProfile->characterArmor.weapon2.weight;
					}

				}
				else
				{
					characterProfile->characterArmor.weapon2 = itemProperties;
					characterProfile->characterArmor.weapon2.isEquipped = true;
					characterProfile->characterArmor.weapon2.weapon2Item = true;
					DecreaseItemFromInventory(characterProfile->characterArmor.weapon2);
					characterProfile->currentInventoryWeight += characterProfile->characterArmor.weapon2.weight;
				}/*_*/
			}
			else
			{
				characterProfile->characterArmor.weapon1 = itemProperties;
				characterProfile->characterArmor.weapon1.isEquipped = true;
				DecreaseItemFromInventory(characterProfile->characterArmor.weapon1);
				characterProfile->currentInventoryWeight += characterProfile->characterArmor.weapon1.weight;
			}
		}//One Handed Weapons End
		else if (itemProperties.WearableType == TwoHandedWeapon)
		{
			if (characterProfile->characterArmor.weapon1.isEquipped)
			{
				PutOffItem(characterProfile, 7);
			}
			if (characterProfile->characterArmor.weapon2.isEquipped)
			{
				PutOffItem(characterProfile, 8);
			}
			characterProfile->characterArmor.weapon1 = itemProperties;
			characterProfile->characterArmor.weapon1.isEquipped = true;
			DecreaseItemFromInventory(characterProfile->characterArmor.weapon1);
			characterProfile->currentInventoryWeight += characterProfile->characterArmor.weapon1.weight;
		}//Two Handede Weapons End
		else if (itemProperties.WearableType == Shield)
		{
			if (characterProfile->characterArmor.weapon1.WearableType == TwoHandedWeapon)
			{
				PutOffItem(characterProfile, 7);
			}
			if (characterProfile->characterArmor.weapon2.isEquipped)
			{
				PutOffItem(characterProfile, 8);
			}
			characterProfile->characterArmor.weapon2 = itemProperties;
			characterProfile->characterArmor.weapon2.isEquipped = true;
			DecreaseItemFromInventory(characterProfile->characterArmor.weapon2);
			characterProfile->currentInventoryWeight += characterProfile->characterArmor.weapon2.weight;
		}
		ResetAnimations();
	}
	//Weapons END **************
	// ******************
	//Armor *******************************************************************
	else if (itemProperties.Category == Armor)
	{
		if (itemProperties.WearableType == Head)
		{
			if (characterProfile->characterArmor.head.isEquipped)
			{
				PutOffItem(characterProfile, 0);
			}
			//hairGroom->SetSkeletalMesh(itemProperties.skeletalMesh);
			if (itemProperties.hideHeadMesh)
			{
				headMesh->SetVisibility(false);
			}

			characterProfile->characterArmor.head = itemProperties;
			characterProfile->characterArmor.head.isEquipped = true;
			DecreaseItemFromInventory(characterProfile->characterArmor.head);
			characterProfile->currentInventoryWeight += characterProfile->characterArmor.head.weight;
		}
		else if (itemProperties.WearableType == Top)
		{
			if (characterProfile->characterArmor.top.isEquipped)
			{
				PutOffItem(characterProfile, 1);
			}
			torsoMesh->SetSkeletalMesh(itemProperties.skeletalMesh);

			characterProfile->characterArmor.top = itemProperties;
			characterProfile->characterArmor.top.isEquipped = true;

			DecreaseItemFromInventory(characterProfile->characterArmor.top);
			characterProfile->currentInventoryWeight += characterProfile->characterArmor.top.weight;
		}
		else if (itemProperties.WearableType == Hand)
		{
			if (characterProfile->characterArmor.hand.isEquipped)
			{
				PutOffItem(characterProfile, 2);
			}
			handsMesh->SetSkeletalMesh(itemProperties.skeletalMesh);

			characterProfile->characterArmor.hand = itemProperties;
			characterProfile->characterArmor.hand.isEquipped = true;
			DecreaseItemFromInventory(characterProfile->characterArmor.hand);
			characterProfile->currentInventoryWeight += characterProfile->characterArmor.hand.weight;
		}
		else if (itemProperties.WearableType == Foot)
		{
			if (characterProfile->characterArmor.foot.isEquipped)
			{
				PutOffItem(characterProfile, 3);
			}
			footsMesh->SetSkeletalMesh(itemProperties.skeletalMesh);

			characterProfile->characterArmor.foot = itemProperties;
			characterProfile->characterArmor.foot.isEquipped = true;
			DecreaseItemFromInventory(characterProfile->characterArmor.foot);
			characterProfile->currentInventoryWeight += characterProfile->characterArmor.foot.weight;
		}
		else if (itemProperties.WearableType == FirstRing)
		{
			if (characterProfile->characterArmor.firstRing.isEquipped)
			{
				PutOffItem(characterProfile, 4);
			}
			characterProfile->characterArmor.firstRing = itemProperties;
			characterProfile->characterArmor.firstRing.isEquipped = true;
			DecreaseItemFromInventory(characterProfile->characterArmor.firstRing);
			characterProfile->currentInventoryWeight += characterProfile->characterArmor.firstRing.weight;
		}
		else if (itemProperties.WearableType == SecondRing)
		{
			if (characterProfile->characterArmor.secondRing.isEquipped)
			{
				PutOffItem(characterProfile, 5);
			}
			characterProfile->characterArmor.secondRing = itemProperties;
			characterProfile->characterArmor.secondRing.isEquipped = true;
			DecreaseItemFromInventory(characterProfile->characterArmor.secondRing);
			characterProfile->currentInventoryWeight += characterProfile->characterArmor.secondRing.weight;
		}
		else if (itemProperties.WearableType == Neck)
		{
			if (characterProfile->characterArmor.neck.isEquipped)
			{
				PutOffItem(characterProfile, 6);
			}
			characterProfile->characterArmor.neck = itemProperties;
			characterProfile->characterArmor.neck.isEquipped = true;
			DecreaseItemFromInventory(characterProfile->characterArmor.neck);
			characterProfile->currentInventoryWeight += characterProfile->characterArmor.neck.weight;
		}
		ResetAnimations();
	}
	//Armor END************
	//************
	//Consumables**************************************************************
	else if (itemProperties.Category == Consumable) 
	{
		//Resets inventory UI
		DecreaseItemFromInventory(itemProperties);
		
		//Consumable Effects ***

		if (itemProperties.ConsumableEffect == Heal)
		{
			characterProfile->characterCurrentHealth += itemProperties.effectStrength;
		}
		else if (itemProperties.ConsumableEffect == DamageHealth)
		{
			characterProfile->characterCurrentHealth -= itemProperties.effectStrength;
		}
		else if (itemProperties.ConsumableEffect == BoostSpeed)
		{
			GetCharacterMovement()->MaxWalkSpeed += itemProperties.effectStrength;
			FTimerHandle effectTimer;
			GetWorldTimerManager().SetTimer(effectTimer, FTimerDelegate::CreateLambda([=]() {
				GetCharacterMovement()->MaxWalkSpeed -= itemProperties.effectStrength;
				}), itemProperties.effectTime, false);
		}

		//******************* Consumable Effects end
	}
	ResetInventoryUI();
}

void APlayerControls::ResetAnimations()
{
	torsoMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	headMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	handsMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	footsMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);

	torsoMesh->Play(true);
	torsoMesh->SetPosition(.0f);

	headMesh->Play(true);
	headMesh->SetPosition(0.f);

	handsMesh->Play(true);
	handsMesh->SetPosition(0.f);

	footsMesh->Play(true);
	footsMesh->SetPosition(0.f);

	torsoMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	headMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	handsMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	footsMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
}

void APlayerControls::ControlFirstCharacter()
{
	ControlNPC(0);
}

void APlayerControls::ControlSecondCharacter()
{
	ControlNPC(1);
}

void APlayerControls::ControlThirdCharacter()
{
	ControlNPC(2);
}

void APlayerControls::ControlFourthCharacter()
{
	ControlNPC(3);

	//characterProfile->charGender = static_cast<FCharacterGender>(1);
}

void APlayerControls::ControlNPC(int index)
{
	if (groupMembers.Find(this) != index && !inDialog && groupMembers.Num() >= index + 1)
	{
		camRotating = false;

		onAIControl = true;
		groupMembers[index]->onAIControl = false;

		mainHUD->RemoveFromParent();
		groupMembers[index]->mainHUD->AddToViewport();

		groupMembers[index]->groupMembers = groupMembers;

		groupMembers[index]->newTargetArmLength = newTargetArmLength;
		groupMembers[index]->springArm->TargetArmLength = springArm->TargetArmLength;

		groupMembers[index]->springArm->bEnableCameraRotationLag = false;
		springArm->bEnableCameraRotationLag = false;
		groupMembers[index]->springArm->SetRelativeRotation(springArm->GetRelativeRotation());

		//Close loot ui if It's enabled
		if (lootObject)
		{
			lootObject->moveToLootObject = false;
			lootObject->DisableLootUI(SelectedActor);
		}

		//Switch controllers
		groupMembers[index]->GetController()->Possess(this);
		playerController->Possess(groupMembers[index]);
		
		//Continue to walk if It's on AI Control before switch
		if (onAIMovement)
		{
			MoveToLocation(actorToBeGone, targetLocation);
		}
		if (groupMembers[index]->onAIMovement)
		{
			groupMembers[index]->MoveToLocation(groupMembers[index]->actorToBeGone, groupMembers[index]->targetLocation);
		}

		//Switch inventory to possessed characters inventory
		if (inventoryEnabled)
		{
			ToggleInventory();
			groupMembers[index]->ToggleInventory();
		}

		//Smooth camera switch
		groupMembers[index]->springArm->TargetOffset.X = (springArm->GetComponentLocation().X - groupMembers[index]->GetActorLocation().X) + springArm->TargetOffset.X;
		groupMembers[index]->springArm->TargetOffset.Y = (springArm->GetComponentLocation().Y - groupMembers[index]->GetActorLocation().Y) + springArm->TargetOffset.Y;
		groupMembers[index]->springArm->TargetOffset.Z = (springArm->GetComponentLocation().Z - groupMembers[index]->GetActorLocation().Z) + springArm->TargetOffset.Z;
		SmoothCameraSwitch(index, 10.f);

		//Enables rotation lag after a second, so camera switch wont look weird
		FTimerHandle enableLagTimer;
		GetWorldTimerManager().SetTimer(enableLagTimer, 
			FTimerDelegate::CreateLambda([=]() 
			{
				groupMembers[index]->springArm->bEnableCameraRotationLag = true;
				springArm->bEnableCameraRotationLag = true;
			}), GetWorld()->GetDeltaSeconds(), false);


		if (findEnemyComponent->nearbyEnemies.Num() > 0)
		{
			StartCombat(nullptr);

			if (groupMembers[index]->actorToBeGone)
			{
				
				AActor* enemy = groupMembers[index]->actorToBeGone;
				groupMembers[index]->GetWorldTimerManager().ClearTimer(groupMembers[index]->pickEnemyTimer);
				groupMembers[index]->StopAIMovement(true);
				groupMembers[index]->actorToBeGone = enemy;
			}
		}
	}

	//Change controlledChar variables in all group members
	if (groupMembers.Num() - 1 >= index)
	{
		controlledChar = groupMembers[index];
		for (int i = 0; i <= groupMembers.Num() - 1; i++)
		{
			groupMembers[i]->controlledChar = controlledChar;
		}

		//Set main players charIndex, so main player can follow possessed npc
		if (index != 0)
		{
			groupMembers[0]->charIndex = groupMembers[index]->charIndex;
		}
		else
		{
			groupMembers[0]->charIndex = 0;
		}
	}

}

void APlayerControls::SmoothCameraSwitch(int index, float moveSpeed)
{
	if (!toNewWorld) //Game crashes if this function works while moving to new world
	{
		groupMembers[index]->springArm->TargetOffset.X = FMath::FInterpTo(groupMembers[index]->springArm->TargetOffset.X, 0, GetWorld()->GetDeltaSeconds(), moveSpeed);
		groupMembers[index]->springArm->TargetOffset.Y = FMath::FInterpTo(groupMembers[index]->springArm->TargetOffset.Y, 0, GetWorld()->GetDeltaSeconds(), moveSpeed);
		groupMembers[index]->springArm->TargetOffset.Z = FMath::FInterpTo(groupMembers[index]->springArm->TargetOffset.Z, 0, GetWorld()->GetDeltaSeconds(), moveSpeed);

		if (FMath::Abs(groupMembers[index]->springArm->TargetOffset.X) < 0.2f &&
			FMath::Abs(groupMembers[index]->springArm->TargetOffset.Y) < 0.2f &&
			FMath::Abs(groupMembers[index]->springArm->TargetOffset.Z) < 0.2f)
		{
			groupMembers[index]->springArm->TargetOffset = FVector(0);
			return;
		}
		else
		{
			FTimerHandle repeatTime;
			GetWorldTimerManager().SetTimer(repeatTime, FTimerDelegate::CreateLambda([=]() {
				SmoothCameraSwitch(index, moveSpeed); 
				}), GetWorld()->GetDeltaSeconds() / 2, false);
		}
	}
}

void APlayerControls::MoveToLocation(const AActor* actor, const FVector& Location)
{
	//UAIBlueprintHelperLibrary::SimpleMoveToLocation(playerController, Location);

	if (!CheckIfAnyUIEnabled() && actor)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), actor);
		onAIMovement = true;
	}
	else if(!CheckIfAnyUIEnabled())
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), Location);
		onAIMovement = true;
	}
}

bool APlayerControls::CheckIfAnyUIEnabled()
{
	if (lootObject)
	{
		if (inDialog || inventoryEnabled || lootObject->lootUIEnabled)
		{
			return true;
		}
	}
	else if (inDialog || inventoryEnabled)
	{
		return false;
	}

	return false;
}

void APlayerControls::StopAIOnInput()
{
	if (onAIMovement)
	{
		if (GetController() == playerController && (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::W) || GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::A)
			|| GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::S) || GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::D)))
		{
			StopAIMovement(true);
		}
		if (lootObject)
		{
			StopAIMovement(lootObject->lootUIEnabled);
		}
		else if (itemRef)
		{
			StopAIMovement(itemTaken);
		}
		if (FVector::Dist(GetActorLocation(), targetLocation) < 120)
		{
			onAIMovement = false;
		}
	}
}

void APlayerControls::StopAIMovement(bool goalDone)
{
	if (goalDone)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), GetActorLocation());
		onAIMovement = false;
		itemTaken = false;
		if (itemRef)
		{
			itemRef->moveToObject = false;
		}
		if (actorToBeGone)
		{
			actorToBeGone = nullptr;
		}
	}
}

void APlayerControls::FollowControlledCharacter()
{
	if (!onAIMovement && onAIControl && inGroup && !inCombat)
	{	
		if (600.f < GetDistanceTo(controlledChar))
		{
			
			FVector desiredLocation;

			if (charIndex == 1)
			{
				desiredLocation = GetPlayerBehindLocation(155, 290);
			}
			else if (charIndex == 2)
			{
				desiredLocation = GetPlayerBehindLocation(200, 0);
			}
			else if (charIndex == 3)
			{
				desiredLocation = GetPlayerBehindLocation(140, -305);
			}

			UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), desiredLocation);
			followingChar = true;
		}
		else if(100.f > GetDistanceTo(controlledChar) && followingChar)
		{
			StopAIMovement(true);
			followingChar = false;
		}
	}
	
}

FVector APlayerControls::GetPlayerBehindLocation(float behind, float right)
{
	FVector actorForwardVector;
	FVector offsetVector;

	actorForwardVector = controlledChar->GetActorForwardVector();
	offsetVector = -actorForwardVector * behind + actorForwardVector.RotateAngleAxis(90, FVector(0, 0, 1)) * right;
	return controlledChar->GetActorLocation() + offsetVector;
}

void APlayerControls::SaveGame()
{
	if (saveSystem)
	{
		int temp = groupMembers[0]->charIndex;
		groupMembers[0]->charIndex = 0;
		saveSystem->SaveGame("Save1");
		groupMembers[0]->charIndex = temp;
	}
}

void APlayerControls::LoadGame()
{
	if (saveSystem)
	{
		saveSystem->LoadGame("Save1");
	}
}

void APlayerControls::Attack(float DeltaTime, AActor* enemyActor) //Melee attack range <= 120, ranged attack range <= 1000
{
	APlayerControls* enemy = Cast<APlayerControls>(enemyActor);
	if ((!characterProfile->characterArmor.weapon1.isEquipped && characterProfile->charClass != Mage) ||
		(characterProfile->characterArmor.weapon1.isEquipped && characterProfile->characterArmor.weapon1.weaponType == Melee))
	{
		if (GetDistanceTo(enemy) <= 120)
		{
			//Moves to enemy
			TurnToEnemy(enemy->GetActorLocation());

			combatCounter += attackSpeed * DeltaTime;
			if (combatCounter >= 2)
			{
				if (!isDamagedEnemy(enemy))
				{
					combatCounter = 0;
					return;
				}

				enemy->ApplyDamage(CalculateDamage(enemy));
				
				UE_LOG(LogTemp, Warning, TEXT("Hit with Melee"));
				UE_LOG(LogTemp, Warning, TEXT("Enemy health: %f"), enemy->characterProfile->characterCurrentHealth);
				combatCounter = 0;
			}
		}
		else
		{
			MoveToLocation(enemy, FVector(0));
		}
	}
	else //Ranged attacks
	{
		FHitResult OutHit;
		FCollisionQueryParams CollisionParams;

		//Ignore Npcs
		TArray<AActor*> ActorsToIgnore;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerControls::StaticClass(), ActorsToIgnore);

		for (AActor* Actor : ActorsToIgnore)
		{
			CollisionParams.AddIgnoredActor(Actor);
		}

		// If there is a barrier between character and target, or if character is too far away from the target then character changes Its position
		if (GetWorld()->LineTraceSingleByChannel(OutHit, GetActorLocation(), enemy->GetActorLocation(), ECC_Visibility, CollisionParams) || GetDistanceTo(enemy) > 1000)
		{
			MoveToLocation(enemy, FVector(0));
		}
		else
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), GetActorLocation());
			TurnToEnemy(enemy->GetActorLocation());

			combatCounter += attackSpeed * DeltaTime;
			if (combatCounter >= 2)
			{
				if (!isDamagedEnemy(enemy))
				{
					combatCounter = 0;
					return;
				}

				enemy->ApplyDamage(CalculateDamage(enemy));

				UE_LOG(LogTemp, Warning, TEXT("Hit with Ranged"));
				UE_LOG(LogTemp, Warning, TEXT("Enemy health: %f"), enemy->characterProfile->characterCurrentHealth);
				combatCounter = 0;
			}
		}
	}

	//if (findEnemyComponent->nearbyEnemies.Contains(enemy) && enemy->characterProfile->characterCurrentHealth <= 0)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("hello"));
	//	//actorToBeGone = nullptr;
	//	findEnemyComponent->nearbyEnemies.Remove(enemy);
	//	GetWorldTimerManager().ClearTimer(pickEnemyTimer);
	//}
}

void APlayerControls::HitFast()
{
	if (characterProfile->characterCurrentEnergy >= 8)
	{
		combatCounter = 2;
		characterProfile->characterCurrentEnergy -= 8;
	}
}

int APlayerControls::CalculateDamage(AActor* enemyActor)
{
	APlayerControls* enemy = Cast<APlayerControls>(enemyActor);
	FTimerHandle hitTime;

	if ((!characterProfile->characterArmor.weapon1.isEquipped && (characterProfile->charClass != Mage)
		|| characterProfile->characterArmor.weapon1.weaponType == Melee))
	{ //Calculates melee damages. Mage characters does ranged attack without a weapon
		if (characterProfile->characterArmor.weapon2.isEquipped)
		{
			//Play two handed sword animation
			return (characterProfile->characterStats.strength *
				(characterProfile->characterArmor.weapon1.damageBonus + enemy->characterProfile->characterArmor.weapon2.damageBonus)) / FMath::RandRange(1, 20);
		}
		else if(characterProfile->characterArmor.weapon1.isEquipped)
		{
			//Play one handed sword animation
			return (characterProfile->characterStats.strength * characterProfile->characterArmor.weapon1.damageBonus) / FMath::RandRange(1, 20);
		}
		else
		{
			//Play punch animation
			punchAnim = true;

			GetWorldTimerManager().SetTimer(hitTime, FTimerDelegate::CreateLambda([=]() {
				punchAnim = false;
				}), 1.9f, false);
			return characterProfile->characterStats.strength * FMath::RandRange(1, 5);
		}
	}
	else
	{
		if (characterProfile->characterArmor.weapon1.damageType == StrDamage)
		{
			//Bow animation
			return (characterProfile->characterStats.strength * characterProfile->characterArmor.weapon1.damageBonus) / FMath::RandRange(1, 20);
		}
		else if(characterProfile->characterArmor.weapon1.isEquipped)
		{
			spellCasting = true;

			GetWorldTimerManager().SetTimer(hitTime, FTimerDelegate::CreateLambda([=]() {
				spellCasting = false;
				}), 1.9f, false);

			return (characterProfile->characterStats.intelligence * characterProfile->characterArmor.weapon1.damageBonus) / FMath::RandRange(1, 20);
		}
		else
		{
			spellCasting = true;
			
			GetWorldTimerManager().SetTimer(hitTime, FTimerDelegate::CreateLambda([=]() {
				spellCasting = false;
				}), 1.9f, false);
			return characterProfile->characterStats.intelligence * FMath::RandRange(1, 5);
		}
	}
	
	return 0;
}

bool APlayerControls::isDamagedEnemy(AActor* enemyActor)
{
	APlayerControls* enemy = Cast<APlayerControls>(enemyActor);
	float hitChance = (float)characterProfile->characterStats.dexterity / ((float)characterProfile->characterStats.dexterity + (float)enemy->characterProfile->characterStats.dexterity);

	// Check if the hit is successful
	if (FMath::RandRange(0.0f, 1.0f) <= hitChance)
	{
		//Successful hit
		return true;
	}
	else
	{
		//Unuccessful hit
		return false;
	}
}

void APlayerControls::NPCInteractions(float DeltaTime)
{
	if (actorToBeGone && actorToBeGone->GetClass()->GetSuperClass()->GetName() == FString("BP_NPC_Management_C"))
	{
		ANPC_Management* npc = Cast<ANPC_Management>(actorToBeGone);
		if (npc->dead)
		{
			actorToBeGone = nullptr;
			return;
		}


		if ((npc->NPCStyle == Hostile || npc->inGroup) && inCombat)
		{//Npcs combat with other npcs
			//if (npc->characterProfile->characterCurrentHealth <= 0)
			//{
			//	inCombat = false;
			//	StopAIMovement(true);
			//}
			Attack(DeltaTime, npc);
		}
		else if (npc->NPCStyle == Talkable && GetDistanceTo(npc) < 250) //To Start a dialog
		{
			npc->StartDialog();

			actorToBeGone = nullptr;
			inDialog = true;
			StopAIMovement(true);
			//Turns to npc
			SetActorRotation((npc->GetActorLocation() - GetActorLocation()).Rotation());
		} 
	}//Main characters combat with npcs
	else if (actorToBeGone && actorToBeGone->GetClass()->GetSuperClass()->GetName() == FString("PlayerControls"))
	{
		if (Cast<ANPC_Management>(this))
		{
			ANPC_Management* npc = Cast<ANPC_Management>(this);
			if (npc->NPCStyle == Hostile)
			{
				APlayerControls* player = Cast<APlayerControls>(actorToBeGone);
				Attack(DeltaTime, actorToBeGone);
			}
		}
	}
}

void APlayerControls::TurnToEnemy(FVector enemyLocation)
{
	// Get the direction to the enemy
	FVector direction = enemyLocation - GetActorLocation();
	direction.Normalize();

	// Calculate the rotation in the Z axis
	float angle = FMath::RadiansToDegrees(FMath::Atan2(direction.Y, direction.X));
	FRotator newRotation = FRotator::MakeFromEuler(FVector(0.0f, 0.0f, angle));

	// Set the actor rotation
	SetActorRotation(newRotation);
}

void APlayerControls::StartCombat(AActor* enemy)
{
	GetWorldTimerManager().ClearTimer(pickEnemyTimer);
	inCombat = true;

	if (enemy && !findEnemyComponent->nearbyEnemies.Contains(enemy))
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s added %s"), *GetName(), *enemy->GetName());
		findEnemyComponent->nearbyEnemies.Add(enemy);
	}
	if (!onAIControl) //If character is not on ai control, character wont choose an enemy automaticly
	{
		if (findEnemyComponent->nearbyEnemies.Num() == 0) //If there are no enemy near to player character then inCombat sets to false
		{
			inCombat = false;
		}
		return;
	}

	//Checks everysecond if there are better target to attack
	GetWorldTimerManager().SetTimer(pickEnemyTimer,
		FTimerDelegate::CreateLambda([=]()
			{
			if (inCombat)
			{
				actorToBeGone = findEnemyComponent->PickEnemy();
				//If there are no enemy stops combat
				if (!actorToBeGone)
				{
					UE_LOG(LogTemp, Warning, TEXT("%s: stopped"), *GetName());
					StopAIMovement(true);
					inCombat = false;

					//Stops loop
					GetWorldTimerManager().ClearTimer(pickEnemyTimer);
				}
			}
		}), 1.0f, true);
}

void APlayerControls::ApplyDamage(int Damage)
{
	characterProfile->characterCurrentHealth -= Damage - int(GetArmorRating() / 5);
}

void APlayerControls::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OverlappedWithActor(OtherActor);
}

void APlayerControls::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != this && findEnemyComponent->nearbyEnemies.Contains(OtherActor))
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s deleted: %s"), *GetName(), *OtherActor->GetName());
		findEnemyComponent->nearbyEnemies.Remove(OtherActor);
		GetWorldTimerManager().ClearTimer(pickEnemyTimer);
		StartCombat(nullptr);
	}
}

void APlayerControls::SkillOne()
{
	if (skills && !skills->skillOneTargeting)
	{
		skills->SkillOne(characterProfile->charClass, this, FVector::ZeroVector);
		skills->skillTwoTargeting = false;
		skills->skillThreeTargeting = false;
	}
	else
	{
		skills->skillOneTargeting = false;
	}
}

void APlayerControls::SkillTwo()
{
	if (skills && !skills->skillTwoTargeting)
	{
		skills->SkillTwo(characterProfile->charClass, this, FVector::ZeroVector);
		skills->skillOneTargeting = false;
		skills->skillThreeTargeting = false;
	}
	else
	{
		skills->skillTwoTargeting = false;
	}
}

void APlayerControls::SkillThree()
{
	if (skills && !skills->skillThreeTargeting)
	{
		skills->SkillThree(characterProfile->charClass, this, FVector::ZeroVector);
		skills->skillOneTargeting = false;
		skills->skillTwoTargeting = false;
	}
	else
	{
		skills->skillThreeTargeting = false;
	}
}

void APlayerControls::SlowTime()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.25f);
}

void APlayerControls::BackToNormalTime()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
}