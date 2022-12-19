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
	SpringArm->TargetArmLength = targetArmLength;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	playerController = UGameplayStatics::GetPlayerController(this, 0);
	
	

}

// Called when the game starts or when spawned
void APlayerControls::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
	GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;
	GetWorld()->GetFirstPlayerController()->bEnableMouseOverEvents = true;

	mainHUD = CreateWidget<UManageWidgets>(UGameplayStatics::GetPlayerController(GetWorld(), 0), mainUI);

	mainHUD->characterStats.playerClass = Mage;
	mainHUD->characterStats.strength = 15;
	mainHUD->characterStats.dexterity = 10;
	mainHUD->characterStats.intelligence = 10;
	mainHUD->characterStats.constitution = 12;
	mainHUD->characterStats.wisdom = 10;
	mainHUD->AddToViewport();

	mainHUD->beginningStats = mainHUD->characterStats;


	//UE_LOG(LogTemp, Warning, TEXT("%f"), HUD->playerCurrentHealth);

	//Calculates Maximum Inventory Capacity
	mainHUD->maxInventoryCapacity = (mainHUD->beginningStats.strength * 10) + ((mainHUD->characterStats.strength - mainHUD->beginningStats.strength) * 2);
	//Calculates Maximum Health
	mainHUD->playerMaximumHealth = (mainHUD->beginningStats.constitution * 10) + ((mainHUD->characterStats.constitution - mainHUD->beginningStats.constitution) * 2);
	mainHUD->playerCurrentHealth = mainHUD->playerMaximumHealth;


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
		int x = 0;
		int y = 0;
		playerController->GetViewportSize(x, y);
		playerController->SetMouseLocation(x/2, y/2);
	}

	if (itemRef)
	{
		if (itemRef->canLoot && itemRef->moveToObject)
		{
			AddItemToInventoryFromGround();
		}
	}


	if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::W) || GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::A) 
		|| GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::S) || GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::D))
	{
		characterOnMove = true;
	}
	else
	{
		characterOnMove = false;
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

	PlayerInputComponent->BindAction("OpenInventory", IE_Pressed, this, &APlayerControls::OpenInventory);

	PlayerInputComponent->BindAxis("CameraZoom", this, &APlayerControls::CameraZoom);
}

void APlayerControls::MoveForward(float value)
{
	if ((Controller != nullptr) && (value != 0))
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

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);
	}
}

void APlayerControls::MoveRight(float value)
{
	if ((Controller != nullptr) && (value != 0))
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


		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

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
			playerInput->BindAxis("Look Right / Left", this, &APawn::AddControllerYawInput);
			playerInput->BindAxis("Look Up / Down", this, &APawn::AddControllerPitchInput);

			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
			GetWorld()->GetFirstPlayerController()->bEnableClickEvents = false;
			GetWorld()->GetFirstPlayerController()->bEnableMouseOverEvents = false;
			camRotating = true;
		}
	}
	else if (!inventoryEnabled)
	{
		playerInput->BindAxis("Look Right / Left", this, &APawn::AddControllerYawInput);
		playerInput->BindAxis("Look Up / Down", this, &APawn::AddControllerPitchInput);

		GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
		GetWorld()->GetFirstPlayerController()->bEnableClickEvents = false;
		GetWorld()->GetFirstPlayerController()->bEnableMouseOverEvents = false;
		camRotating = true;
	}
}
void APlayerControls::StopCameraRotation()
{
	if (camRotating)
	{
		playerInput->AxisBindings.Pop();
		playerInput->AxisBindings.Pop();

		GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
		GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;
		GetWorld()->GetFirstPlayerController()->bEnableMouseOverEvents = true;
		camRotating = false;
	}
}

void APlayerControls::CameraZoom(float value)
{
	if (!inventoryEnabled)
	{
		newTargetArmLength -= value * 100;
		newTargetArmLength = FMath::Clamp(newTargetArmLength, 150.f, 1200.f);
		SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, newTargetArmLength, GetWorld()->GetDeltaSeconds(), 5.f);
	}
}

void APlayerControls::OnMouseClick()
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
		}
		else if (*SelectedActor->GetClass()->GetSuperClass()->GetName() == FName("BP_MasterItem_C"))//itemRef
		{
			itemRef = Cast<AMasterItem>(SelectedActor);
			AddItemToInventoryFromGround();
		}
	}

}

void APlayerControls::OpenInventory()
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
	//itemRef = Cast<AMasterItem>(SelectedActor);
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
	
	
	if (mainHUD->currentInventoryWeight + itemProperties.weight <= mainHUD->maxInventoryCapacity)
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
		mainHUD->currentInventoryWeight += itemProperties.weight;
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

	mainHUD->currentInventoryWeight -= itemProperties.weight;
}


void APlayerControls::PutOffItem(UManageWidgets* itemProperties, int WearableSlotIndex)
{
	if (WearableSlotIndex + 1 == Head && itemProperties->characterArmor.head.isEquipped) //Head (This ranking is determined within AddWearableSlots in WP_Inventory Blueprint.)
	{
		
		AddItemToInventory(itemProperties->characterArmor.head);
		itemProperties->characterArmor.head.isEquipped = false;
		mainHUD->currentInventoryWeight -= itemProperties->characterArmor.head.weight;
		mainHUD->characterArmor.head.armorBonus = 0;
	}
	else if (WearableSlotIndex + 1 == Top && itemProperties->characterArmor.top.isEquipped) //Top
	{
		AddItemToInventory(itemProperties->characterArmor.top);
		itemProperties->characterArmor.top.isEquipped = false;
		mainHUD->currentInventoryWeight -= itemProperties->characterArmor.top.weight;
		mainHUD->characterArmor.top.armorBonus = 0;
	}
	else if (WearableSlotIndex + 1 == Hand && itemProperties->characterArmor.hand.isEquipped) //Hand
	{
		AddItemToInventory(itemProperties->characterArmor.hand);
		itemProperties->characterArmor.hand.isEquipped = false;
		mainHUD->currentInventoryWeight -= itemProperties->characterArmor.hand.weight;
		mainHUD->characterArmor.hand.armorBonus = 0;
	}
	else if (WearableSlotIndex + 1 == Foot && itemProperties->characterArmor.foot.isEquipped) //Foot
	{
		AddItemToInventory(itemProperties->characterArmor.foot);
		itemProperties->characterArmor.foot.isEquipped = false;
		mainHUD->currentInventoryWeight -= itemProperties->characterArmor.foot.weight;
		mainHUD->characterArmor.foot.armorBonus = 0;
	}
	else if (WearableSlotIndex + 1 == FirstRing && itemProperties->characterArmor.firstRing.isEquipped) //Ring1
	{
		AddItemToInventory(itemProperties->characterArmor.firstRing);
		itemProperties->characterArmor.firstRing.isEquipped = false;
		mainHUD->currentInventoryWeight -= itemProperties->characterArmor.firstRing.weight;
		mainHUD->characterArmor.firstRing.armorBonus = 0;
	}
	else if (WearableSlotIndex + 1 == SecondRing && itemProperties->characterArmor.secondRing.isEquipped) //Ring2
	{
		AddItemToInventory(itemProperties->characterArmor.secondRing);
		itemProperties->characterArmor.secondRing.isEquipped = false;
		mainHUD->currentInventoryWeight -= itemProperties->characterArmor.secondRing.weight;
		mainHUD->characterArmor.secondRing.armorBonus = 0;
	}
	else if (WearableSlotIndex + 1 == Neck && itemProperties->characterArmor.neck.isEquipped) //Neck
	{
		AddItemToInventory(itemProperties->characterArmor.neck);
		itemProperties->characterArmor.neck.isEquipped = false;
		mainHUD->currentInventoryWeight -= itemProperties->characterArmor.neck.weight;
		mainHUD->characterArmor.neck.armorBonus = 0;
	}
	ResetInventoryUI();
}

int APlayerControls::GetArmorRating()
{
	int totalArmorRating = 0;
	totalArmorRating += mainHUD->characterArmor.head.armorBonus;
	totalArmorRating += mainHUD->characterArmor.top.armorBonus;
	totalArmorRating += mainHUD->characterArmor.hand.armorBonus;
	totalArmorRating += mainHUD->characterArmor.neck.armorBonus;
	totalArmorRating += mainHUD->characterArmor.firstRing.armorBonus;
	totalArmorRating += mainHUD->characterArmor.secondRing.armorBonus;
	totalArmorRating += mainHUD->characterArmor.foot.armorBonus;
	return totalArmorRating;
}

void APlayerControls::ItemInteraction(FItemProperties itemProperties) //Called in Item Slot blueprint
{
	// Weapon *******************************************************************
	if (itemProperties.Category == Weapon) 
	{
		if (itemProperties.WearableType == OneHandedWeapon)
		{
			if (mainHUD->characterArmor.weapon1.isEquipped)
			{
				if (mainHUD->characterArmor.weapon2.isEquipped)
				{
					if (mainHUD->characterArmor.weapon2.WearableType == OneHandedWeapon) //If weapon2 slots has a onehandedwaepon then put off it and wear new selected wearable
					{
						//UE_LOG(LogTemp, Warning, TEXT("1"));
						PutOffItem(mainHUD, 8);
						mainHUD->characterArmor.weapon2 = itemProperties;
						mainHUD->characterArmor.weapon2.isEquipped = true;
						mainHUD->characterArmor.weapon2.weapon2Item = true;
						DecreaseItemFromInventory(mainHUD->characterArmor.weapon2);
						mainHUD->currentInventoryWeight += mainHUD->characterArmor.weapon2.weight;
					}
					else
					{
						//UE_LOG(LogTemp, Warning, TEXT("2"));
						PutOffItem(mainHUD, 7);
						mainHUD->characterArmor.weapon1 = itemProperties;
						mainHUD->characterArmor.weapon1.isEquipped = true;
						DecreaseItemFromInventory(mainHUD->characterArmor.weapon1);
						mainHUD->currentInventoryWeight += mainHUD->characterArmor.weapon1.weight;
					}

				}
				else
				{
					//UE_LOG(LogTemp, Warning, TEXT("Weapon2 One handed weapon equipped"));
					mainHUD->characterArmor.weapon2 = itemProperties;
					mainHUD->characterArmor.weapon2.isEquipped = true;
					mainHUD->characterArmor.weapon2.weapon2Item = true;
					DecreaseItemFromInventory(mainHUD->characterArmor.weapon2);
					mainHUD->currentInventoryWeight += mainHUD->characterArmor.weapon2.weight;
				}/*_*/
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("Weapon1 One handed weapon equipped"));
				mainHUD->characterArmor.weapon1 = itemProperties;
				mainHUD->characterArmor.weapon1.isEquipped = true;
				DecreaseItemFromInventory(mainHUD->characterArmor.weapon1);
				mainHUD->currentInventoryWeight += mainHUD->characterArmor.weapon1.weight;
			}
		}//One Handed Weapons End
		else if (itemProperties.WearableType == TwoHandedWeapon)
		{

		}

		ResetInventoryUI();
	}
	//Weapons END **************
	// ******************
	//Armor *******************************************************************
	else if (itemProperties.Category == Armor) 
	{
		if (itemProperties.WearableType == Head)
		{
			if (mainHUD->characterArmor.head.isEquipped)
			{
				PutOffItem(mainHUD, 0);
			}
			UE_LOG(LogTemp, Warning, TEXT("test"));
			mainHUD->characterArmor.head = itemProperties;
			mainHUD->characterArmor.head.isEquipped = true;
			DecreaseItemFromInventory(mainHUD->characterArmor.head);
			mainHUD->currentInventoryWeight += mainHUD->characterArmor.head.weight;
		}
		else if (itemProperties.WearableType == Top)
		{
			if (mainHUD->characterArmor.top.isEquipped)
			{
				PutOffItem(mainHUD, 1);
			}
			mainHUD->characterArmor.top = itemProperties;
			mainHUD->characterArmor.top.isEquipped = true;
			DecreaseItemFromInventory(mainHUD->characterArmor.top);
			mainHUD->currentInventoryWeight += mainHUD->characterArmor.top.weight;
		}
		else if (itemProperties.WearableType == Hand)
		{
			if (mainHUD->characterArmor.hand.isEquipped)
			{
				PutOffItem(mainHUD, 2);
			}
			mainHUD->characterArmor.hand = itemProperties;
			mainHUD->characterArmor.hand.isEquipped = true;
			DecreaseItemFromInventory(mainHUD->characterArmor.hand);
			mainHUD->currentInventoryWeight += mainHUD->characterArmor.hand.weight;
		}
		else if (itemProperties.WearableType == Foot)
		{
			if (mainHUD->characterArmor.foot.isEquipped)
			{
				PutOffItem(mainHUD, 3);
			}
			mainHUD->characterArmor.foot = itemProperties;
			mainHUD->characterArmor.foot.isEquipped = true;
			DecreaseItemFromInventory(mainHUD->characterArmor.foot);
			mainHUD->currentInventoryWeight += mainHUD->characterArmor.foot.weight;
		}
		else if (itemProperties.WearableType == FirstRing)
		{
			if (mainHUD->characterArmor.firstRing.isEquipped)
			{
				PutOffItem(mainHUD, 4);
			}
			mainHUD->characterArmor.firstRing = itemProperties;
			mainHUD->characterArmor.firstRing.isEquipped = true;
			DecreaseItemFromInventory(mainHUD->characterArmor.firstRing);
			mainHUD->currentInventoryWeight += mainHUD->characterArmor.firstRing.weight;
		}
		else if (itemProperties.WearableType == SecondRing)
		{
			if (mainHUD->characterArmor.secondRing.isEquipped)
			{
				PutOffItem(mainHUD, 5);
			}
			mainHUD->characterArmor.secondRing = itemProperties;
			mainHUD->characterArmor.secondRing.isEquipped = true;
			DecreaseItemFromInventory(mainHUD->characterArmor.secondRing);
			mainHUD->currentInventoryWeight += mainHUD->characterArmor.secondRing.weight;
		}
		else if (itemProperties.WearableType == Neck)
		{
			if (mainHUD->characterArmor.neck.isEquipped)
			{
				PutOffItem(mainHUD, 6);
			}
			mainHUD->characterArmor.neck = itemProperties;
			mainHUD->characterArmor.neck.isEquipped = true;
			DecreaseItemFromInventory(mainHUD->characterArmor.neck);
			mainHUD->currentInventoryWeight += mainHUD->characterArmor.neck.weight;
		}

		ResetInventoryUI();
	}
	//Armor END************
	//************
	//Consumables**************************************************************
	else if (itemProperties.Category == Consumable) 
	{
		//Resets inventory UI
		DecreaseItemFromInventory(itemProperties);
		ResetInventoryUI();
		//Consumable Effects ***

		if (itemProperties.ConsumableEffect == Heal)
		{
			mainHUD->playerCurrentHealth += itemProperties.effectStrength;
		}
		else if (itemProperties.ConsumableEffect == DamageHealth)
		{
			mainHUD->playerCurrentHealth -= itemProperties.effectStrength;
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
}


