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

	HUD = CreateWidget<UManageWidgets>(UGameplayStatics::GetPlayerController(GetWorld(), 0), mainUI);
	HUD->AddToViewport();

	//UE_LOG(LogTemp, Warning, TEXT("%f"), HUD->playerCurrentHealth);

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
		lootObject->DisableLootUI(SelectedActor);

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
		lootObject->DisableLootUI(SelectedActor);

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, value);
	}
}

void APlayerControls::StartCameraRotation()
{
	if (!inventoryEnabled)
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
	FHitResult HitResult;
	playerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, HitResult);

	lootObject->DisableLootUI(SelectedActor);

	SelectedActor = HitResult.GetActor();

	if (SelectedActor)
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s"), *SelectedActor->GetClass()->GetSuperClass()->GetName());
		
		//Open Loot
		if (*SelectedActor->GetClass()->GetSuperClass()->GetName() == FName("BP_LootObject_C"))
		{
			lootObject = Cast<ALootObject>(SelectedActor);
			lootObject->EnableLootUI();
		}

		//itemRef
		if (*SelectedActor->GetClass()->GetSuperClass()->GetName() == FName("BP_MasterItem_C"))
		{
			AddItemToInventoryFromGround(Cast<AMasterItem>(SelectedActor));
		}
	}

}

void APlayerControls::OpenInventory()
{
	if (inventoryUI && !inventoryEnabled)
	{
		lootObject->DisableLootUI(SelectedActor);

		HUD = CreateWidget<UManageWidgets>(UGameplayStatics::GetPlayerController(GetWorld(), 0), inventoryUI);
		HUD->AddToViewport();

		inventoryEnabled = true;
		UE_LOG(LogTemp, Warning, TEXT("Inventory Opened"));
	}
	else if(inventoryUI && inventoryEnabled)
	{
		HUD->RemoveFromViewport();

		inventoryEnabled = false;
		UE_LOG(LogTemp, Warning, TEXT("Inventory Closed"));
	}
}

void APlayerControls::AddItemToInventoryFromGround(AMasterItem* itemRef)
{
	//itemRef = Cast<AMasterItem>(SelectedActor);
	if (itemRef->canLoot)
	{
		AddItemToInventory(itemRef->ItemProperties);
		itemRef->Destroy();
	}
}

void APlayerControls::ItemTransfer(FItemProperties itemProperties)
{
	int itemIndex = 0;
	if (itemProperties.inInventory)
	{
		lootObject->AddItemToLoot(itemProperties);

		
		for (FItemProperties el : inventory)
		{
			if (el.name == itemProperties.name)
			{
				if (el.currentAmount > 1)
				{
					inventory[itemIndex].currentAmount--;
					break;
				}
				else
				{
					inventory.RemoveAt(itemIndex);
					break;
				}
			}
			itemIndex++;
		}
	}
	else
	{
		AddItemToInventory(itemProperties);

		for (FItemProperties el : lootObject->storage)
		{
			if (el.name == itemProperties.name)
			{
				if (el.currentAmount > 1)
				{
					lootObject->storage[itemIndex].currentAmount--;
					break;
				}
				else
				{
					lootObject->storage.RemoveAt(itemIndex);
					break;
				}
			}
			itemIndex++;
		}
	}
}

void APlayerControls::AddItemToInventory(FItemProperties itemProperties)
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
}


