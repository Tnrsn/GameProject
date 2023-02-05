// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControls.h"

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


	headMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Head Mesh");
	headMesh->SetupAttachment(RootComponent);

	handsMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Hand Mesh");
	handsMesh->SetupAttachment(RootComponent);

	footsMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Foot Mesh");
	footsMesh->SetupAttachment(RootComponent);

	torsoMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Torso Mesh");
	torsoMesh->SetupAttachment(RootComponent);

	hairMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Hair Mesh");
	hairMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APlayerControls::BeginPlay()
{
	Super::BeginPlay();
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *Controller->GetName());
	
	if (GetWorld())
	{
		saveSystem = GetWorld()->GetSubsystem<USaveSystem>();
	}

	playerController = UGameplayStatics::GetPlayerController(this, 0);
	characterProfile = NewObject<UCharacterProfiles>();

	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
	GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;
	GetWorld()->GetFirstPlayerController()->bEnableMouseOverEvents = true;

	if (*GetWorld()->GetName() != FName("MainMenu") && *GetWorld()->GetName() != FName("CharacterCreationMenu"))
	{
		inMenu = false;
		InitCharacter();
	}
	else
	{
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

	//if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::F) && GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::F)&&)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("test"));
	//}

	if (camRotating)
	{
		springArm->RotateCamera(playerController);
	}

	if (itemRef)
	{
		if (itemRef->canLoot && itemRef->moveToObject)
		{
			AddItemToInventoryFromGround();
		}
	}

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

	FollowControlledCharacter();

	//UE_LOG(LogTemp, Warning, TEXT("%f"), GetVelocity().Size());
	//UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), FVector(0));
}

void APlayerControls::InitCharacter()
{
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *GetName());

	mainHUD = CreateWidget<UManageWidgets>(UGameplayStatics::GetPlayerController(GetWorld(), 0), mainUI);
	currentLevelName = GetWorld()->GetName();

	if (groupMembers.Num() == 0 && GetController() == playerController)
	{
		groupMembers.Add(Cast<APlayerControls>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)));
		inGroup = true;
		onAIControl = false;
		charIndex = 0;

		mainHUD->characterProfiles = NewObject<UCharacterProfiles>();

		mainHUD->characterProfiles = characterProfile;
		mainHUD->AddToViewport();

		characterProfile->beginningStats = characterProfile->characterStats;

		//Calculates Maximum Inventory Capacity
		characterProfile->maxInventoryCapacity = (characterProfile->beginningStats.strength * 10) + ((characterProfile->characterStats.strength - characterProfile->beginningStats.strength) * 2);
		//Calculates Maximum Health
		characterProfile->characterMaximumHealth = (characterProfile->beginningStats.constitution * 10) + ((characterProfile->characterStats.constitution - characterProfile->beginningStats.constitution) * 2);
		characterProfile->characterCurrentHealth = characterProfile->characterMaximumHealth;
	}

	if (inGroup)
	{
		controlledChar = Cast<APlayerControls>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		controlledCharIndex = 0;
	}
	

}


// Called to bind functionality to input
void APlayerControls::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	playerInput = PlayerInputComponent;


	PlayerInputComponent->BindAxis("MoveForward / Backward", this, &APlayerControls::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &APlayerControls::MoveRight);

	PlayerInputComponent->BindAction("RightClick", IE_Pressed, this, &APlayerControls::StartCameraRotation);
	PlayerInputComponent->BindAction("RightClick", IE_Released, this, &APlayerControls::StopCameraRotation);
	PlayerInputComponent->BindAction("LeftClick", IE_Pressed, this, &APlayerControls::OnMouseClick);

	PlayerInputComponent->BindAction("ToggleInventory", IE_Pressed, this, &APlayerControls::ToggleInventory);

	PlayerInputComponent->BindAxis("CameraZoom", this, &APlayerControls::CameraZoom);

	PlayerInputComponent->BindAction("PassToFirstCharacter", IE_Pressed, this, &APlayerControls::ControlFirstCharacter);
	PlayerInputComponent->BindAction("PassToSecondCharacter", IE_Pressed, this, &APlayerControls::ControlSecondCharacter);
	PlayerInputComponent->BindAction("PassToThirdCharacter", IE_Pressed, this, &APlayerControls::ControlThirdCharacter);
	PlayerInputComponent->BindAction("PassToFourthCharacter", IE_Pressed, this, &APlayerControls::ControlFourthCharacter);

	PlayerInputComponent->BindAction("QuickSave", IE_Pressed, this, &APlayerControls::SaveGame);
	PlayerInputComponent->BindAction("QuickLoad", IE_Pressed, this, &APlayerControls::LoadGame);
}

void APlayerControls::MoveForward(float value)
{
	if ((Controller != nullptr) && (value != 0) && (!inDialog))
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
	if ((Controller != nullptr) && (value != 0) && (!inDialog))
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
		//UE_LOG(LogTemp, Warning, TEXT("%s"), *SelectedActor->GetClass()->GetSuperClass()->GetName());
		if (*SelectedActor->GetClass()->GetSuperClass()->GetName() == FName("BP_LootObject_C"))//Open Loot
		{
			lootObject = Cast<ALootObject>(SelectedActor);
			lootObject->EnableLootUI();

			actorToBeGone = SelectedActor;
			MoveToLocation(lootObject, FVector(0));
		}
		else if (*SelectedActor->GetClass()->GetSuperClass()->GetName() == FName("BP_MasterItem_C"))//itemRef
		{
			itemRef = Cast<AMasterItem>(SelectedActor);
			AddItemToInventoryFromGround();

			actorToBeGone = SelectedActor;
			MoveToLocation(itemRef, FVector(0));
		}
		else if (*SelectedActor->GetClass()->GetSuperClass()->GetName() == FName("BP_NPC_Management_C"))
		{
			inDialog = true;

			actorToBeGone = SelectedActor;
			MoveToLocation(actorToBeGone, FVector(0));
		}
		else
		{
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
				UE_LOG(LogTemp, Warning, TEXT("Inventory Opened"));
			}
			else if (inventoryEnabled)
			{
				inventoryHUD->RemoveFromParent();

				inventoryEnabled = false;
				UE_LOG(LogTemp, Warning, TEXT("Inventory Closed"));
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
			UE_LOG(LogTemp, Warning, TEXT("Inventory Opened"));
		}
		else if (inventoryEnabled)
		{
			inventoryHUD->RemoveFromParent();

			inventoryEnabled = false;
			UE_LOG(LogTemp, Warning, TEXT("Inventory Closed"));
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
		headMesh->SetVisibility(true);
		if (hairBodyMesh)
		{
			hairMesh->SetSkeletalMesh(hairBodyMesh);
		}
		else
		{
			hairMesh->SetSkeletalMesh(NULL);
		}

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
					//UE_LOG(LogTemp, Warning, TEXT("Weapon2 One handed weapon equipped"));
					characterProfile->characterArmor.weapon2 = itemProperties;
					characterProfile->characterArmor.weapon2.isEquipped = true;
					characterProfile->characterArmor.weapon2.weapon2Item = true;
					DecreaseItemFromInventory(characterProfile->characterArmor.weapon2);
					characterProfile->currentInventoryWeight += characterProfile->characterArmor.weapon2.weight;
				}/*_*/
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("Weapon1 One handed weapon equipped"));
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
			hairMesh->SetSkeletalMesh(itemProperties.skeletalMesh);
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

		FTimerHandle enableLagTimer;
		GetWorldTimerManager().SetTimer(enableLagTimer, 
			FTimerDelegate::CreateLambda([=]() 
			{
				groupMembers[index]->springArm->bEnableCameraRotationLag = true;
				springArm->bEnableCameraRotationLag = true;
			}), GetWorld()->GetDeltaSeconds(), false);
	}

	//Change controlledChar variables in all group members
	if (groupMembers.Num() - 1 >= index)
	{
		controlledChar = groupMembers[index];
		controlledCharIndex = index;
		for (int i = 0; i <= groupMembers.Num() - 1; i++)
		{
			groupMembers[i]->controlledChar = controlledChar;
			groupMembers[i]->controlledCharIndex = index;
		}

		//Switch characterIndex
		int temp;
		temp = groupMembers[index]->charIndex;
		groupMembers[index]->charIndex = charIndex;
		charIndex = temp;
	}

}


void APlayerControls::SmoothCameraSwitch(int index, float moveSpeed)
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
	}
}

void APlayerControls::FollowControlledCharacter()
{
	if (!onAIMovement && onAIControl && inGroup)
	{
		
		if (!GetController())
		{//AI uses players controller Its an error******************-*-*-*-************************************!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//Controller = GetWorld()->SpawnActor<AAIController>();
			//UE_LOG(LogTemp, Warning, TEXT("test"));
		}
			
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


			//UE_LOG(LogTemp, Warning, TEXT("%s"), *GetController()->GetName());
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
		//UGameplayStatics::OpenLevel(GetWorld(), FName("DevLevel2"));

		saveSystem->SaveGame();
	}
}

void APlayerControls::LoadGame()
{
	if (saveSystem)
	{
		saveSystem->LoadGame();
	}
}