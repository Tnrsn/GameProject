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

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 250;
	SpringArm->bUsePawnControlRotation = false;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	//AIController = CreateDefaultSubobject<AAIController>("CharacterAIController");
}

// Called when the game starts or when spawned
void APlayerControls::BeginPlay()
{
	Super::BeginPlay();

	if (groupMembers.Num() == 0)
	{
		groupMembers.Add(Cast<APlayerControls>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)));
	}

	playerController = UGameplayStatics::GetPlayerController(this, 0);
	//AIController = NewObject<APathFindingSystem>(this, "AIController");
	//AIController->Possess(this);

	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
	GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;
	GetWorld()->GetFirstPlayerController()->bEnableMouseOverEvents = true;

	mainHUD = CreateWidget<UManageWidgets>(UGameplayStatics::GetPlayerController(GetWorld(), 0), mainUI);
	mainHUD->characterProfiles = NewObject<UCharacterProfiles>();

	characterProfile = NewObject<UCharacterProfiles>();
	mainHUD->characterProfiles = characterProfile;
	mainHUD->AddToViewport();

	characterProfile->characterStats.playerClass = Mage;
	characterProfile->characterStats.strength = 15;
	characterProfile->characterStats.dexterity = 10;
	characterProfile->characterStats.intelligence = 10;
	characterProfile->characterStats.constitution = 12;
	characterProfile->characterStats.wisdom = 10;

	characterProfile->beginningStats = characterProfile->characterStats;

	//Calculates Maximum Inventory Capacity
	characterProfile->maxInventoryCapacity = (characterProfile->beginningStats.strength * 10) + ((characterProfile->characterStats.strength - characterProfile->beginningStats.strength) * 2);
	//Calculates Maximum Health
	characterProfile->characterMaximumHealth = (characterProfile->beginningStats.constitution * 10) + ((characterProfile->characterStats.constitution - characterProfile->beginningStats.constitution) * 2);
	characterProfile->characterCurrentHealth = characterProfile->characterMaximumHealth;

	//if (AIController)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("%s"), *AIController->GetName());
	//	playerController->UnPossess();
	//	AIController->Possess(this);
	//	UE_LOG(LogTemp, Warning, TEXT("heyy"));
	//UAIBlueprintHelperLibrary::SimpleMoveToLocation(playerController, FVector(0));
	//}
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
		RotateCamera();
	}

	if (itemRef)
	{
		if (itemRef->canLoot && itemRef->moveToObject)
		{
			AddItemToInventoryFromGround();
		}
	}


	if (onAIControl)
	{
		if (GetController() == playerController && (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::W) || GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::A)
			|| GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::S) || GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::D)))
		{
			StopAIControl(true);
		}
		if (lootObject)
		{
			StopAIControl(lootObject->lootUIEnabled);
		}
		else if (itemRef)
		{
			StopAIControl(itemTaken);
		}
	}

	//UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), FVector(0));
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
		//const FRotator YawRotation(0, Rotation.Yaw, 0);
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
		//const FRotator YawRotation(0, Rotation.Yaw, 0);
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
			//playerInput->BindAxis("Look Right / Left", this, &APawn::AddControllerYawInput);
			//playerInput->BindAxis("Look Up / Down", this, &APawn::AddControllerPitchInput);

			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
			GetWorld()->GetFirstPlayerController()->bEnableClickEvents = false;
			GetWorld()->GetFirstPlayerController()->bEnableMouseOverEvents = false;
			camRotating = true;
			rotationStarted = true;
		}
	}
	else if (!inventoryEnabled)
	{
		//playerInput->BindAxis("Look Right / Left", this, &APawn::AddControllerYawInput);
		//playerInput->BindAxis("Look Up / Down", this, &APawn::AddControllerPitchInput);

		GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
		GetWorld()->GetFirstPlayerController()->bEnableClickEvents = false;
		GetWorld()->GetFirstPlayerController()->bEnableMouseOverEvents = false;
		camRotating = true;
		rotationStarted = true;
	}
}
void APlayerControls::StopCameraRotation()
{
	if (camRotating)
	{
		//playerInput->AxisBindings.Pop();
		//playerInput->AxisBindings.Pop();

		GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
		GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;
		GetWorld()->GetFirstPlayerController()->bEnableMouseOverEvents = true;
		camRotating = false;
	}
}

void APlayerControls::RotateCamera()
{
	playerController->GetMousePosition(mousePosition.X, mousePosition.Y);

	if (rotationStarted)
	{
		previousMousePosition = mousePosition;
		rotationStarted = false;
	}

	mouseDelta = mousePosition - previousMousePosition;

	FRotator NewRotation = SpringArm->GetComponentRotation();
	NewRotation.Yaw += mouseDelta.X * mouseSensitivity;
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch - mouseDelta.Y * mouseSensitivity, -70.0f, 30.0f); //Camera rotation limits 

	SpringArm->SetWorldRotation(NewRotation);

	previousMousePosition = mousePosition;
}

void APlayerControls::CameraZoom(float value)
{
	if (!inventoryEnabled && !inDialog)
	{
		newTargetArmLength -= value * 100;
		newTargetArmLength = FMath::Clamp(newTargetArmLength, 150.f, 1200.f);
		SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, newTargetArmLength, GetWorld()->GetDeltaSeconds(), 5.f);
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
		characterProfile->currentInventoryWeight -= itemProperties->characterArmor.head.weight;
		AddItemToInventory(itemProperties->characterArmor.head);
		itemProperties->characterArmor.head = FItemProperties();
	}
	else if (WearableSlotIndex + 1 == Top && itemProperties->characterArmor.top.isEquipped) //Top
	{
		characterProfile->currentInventoryWeight -= itemProperties->characterArmor.top.weight;
		AddItemToInventory(itemProperties->characterArmor.top);
		itemProperties->characterArmor.top = FItemProperties();
	}
	else if (WearableSlotIndex + 1 == Hand && itemProperties->characterArmor.hand.isEquipped) //Hand
	{
		characterProfile->currentInventoryWeight -= itemProperties->characterArmor.hand.weight;
		AddItemToInventory(itemProperties->characterArmor.hand);
		itemProperties->characterArmor.hand = FItemProperties();
	}
	else if (WearableSlotIndex + 1 == Foot && itemProperties->characterArmor.foot.isEquipped) //Foot
	{
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

void APlayerControls::ItemInteraction(FItemProperties itemProperties) //Called in Item Slot blueprint
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

		mainHUD->RemoveFromParent();
		groupMembers[index]->mainHUD->AddToViewport();

		groupMembers[index]->groupMembers = groupMembers;

		groupMembers[index]->newTargetArmLength = newTargetArmLength;
		groupMembers[index]->SpringArm->TargetArmLength = SpringArm->TargetArmLength;

		groupMembers[index]->SpringArm->bEnableCameraRotationLag = false;
		SpringArm->bEnableCameraRotationLag = false;
		groupMembers[index]->SpringArm->SetRelativeRotation(SpringArm->GetRelativeRotation());

		if (lootObject)
		{
			lootObject->moveToLootObject = false;
			lootObject->DisableLootUI(SelectedActor);
		}

		groupMembers[index]->GetController()->Possess(this);
		playerController->Possess(groupMembers[index]);

		if (onAIControl)
		{
			MoveToLocation(actorToBeGone, targetLocation);
		}
		if (groupMembers[index]->onAIControl)
		{
			UE_LOG(LogTemp, Warning, TEXT("hey"));
			groupMembers[index]->MoveToLocation(groupMembers[index]->actorToBeGone, groupMembers[index]->targetLocation);
		}

		if (inventoryEnabled)
		{
			ToggleInventory();
			groupMembers[index]->ToggleInventory();
		}

		//Smooth camera switch
		groupMembers[index]->SpringArm->TargetOffset.X = (SpringArm->GetComponentLocation().X - groupMembers[index]->GetActorLocation().X) + SpringArm->TargetOffset.X;
		groupMembers[index]->SpringArm->TargetOffset.Y = (SpringArm->GetComponentLocation().Y - groupMembers[index]->GetActorLocation().Y) + SpringArm->TargetOffset.Y;
		groupMembers[index]->SpringArm->TargetOffset.Z = (SpringArm->GetComponentLocation().Z - groupMembers[index]->GetActorLocation().Z) + SpringArm->TargetOffset.Z;
		SmoothCameraSwitch(index, 5.f);

		FTimerHandle enableLagTimer;
		GetWorldTimerManager().SetTimer(enableLagTimer, 
			FTimerDelegate::CreateLambda([=]() 
			{
				groupMembers[index]->SpringArm->bEnableCameraRotationLag = true;
				SpringArm->bEnableCameraRotationLag = true;
			}), GetWorld()->GetDeltaSeconds(), false);
	}
}


void APlayerControls::SmoothCameraSwitch(int index, float moveSpeed)
{
	groupMembers[index]->SpringArm->TargetOffset.X = FMath::FInterpTo(groupMembers[index]->SpringArm->TargetOffset.X, 0, GetWorld()->GetDeltaSeconds(), moveSpeed);
	groupMembers[index]->SpringArm->TargetOffset.Y = FMath::FInterpTo(groupMembers[index]->SpringArm->TargetOffset.Y, 0, GetWorld()->GetDeltaSeconds(), moveSpeed);
	groupMembers[index]->SpringArm->TargetOffset.Z = FMath::FInterpTo(groupMembers[index]->SpringArm->TargetOffset.Z, 0, GetWorld()->GetDeltaSeconds(), moveSpeed);

	if (FMath::Abs(groupMembers[index]->SpringArm->TargetOffset.X) < 0.2f &&
		FMath::Abs(groupMembers[index]->SpringArm->TargetOffset.Y) < 0.2f &&
		FMath::Abs(groupMembers[index]->SpringArm->TargetOffset.Z) < 0.2f)
	{
		groupMembers[index]->SpringArm->TargetOffset = FVector(0);
		return;
	}
	else
	{
		FTimerHandle repeatTime;
		GetWorldTimerManager().SetTimer(repeatTime, FTimerDelegate::CreateLambda([=]() {
			SmoothCameraSwitch(index, moveSpeed); 
			}), GetWorld()->GetDeltaSeconds(), false);
	}
}

void APlayerControls::MoveToLocation(const AActor* actor, const FVector& Location)
{
	//UAIBlueprintHelperLibrary::SimpleMoveToLocation(playerController, Location);

	if (!CheckIfAnyUIEnabled() && actor)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), actor);
		onAIControl = true;
	}
	else if(!CheckIfAnyUIEnabled())
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), Location);
		onAIControl = true;
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

void APlayerControls::StopAIControl(bool goalDone)
{
	if (goalDone)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), GetActorLocation());
		onAIControl = false;
		itemTaken = false;
		if (itemRef)
		{
			itemRef->moveToObject = false;
		}
	}
}