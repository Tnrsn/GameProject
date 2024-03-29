// Fill out your copyright notice in the Description page of Project Settings.


#include "LootObject.h"

// Sets default values
ALootObject::ALootObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SphereCollision->InitSphereRadius(100);
	RootComponent = SphereCollision;

	TriggerCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger Sphere"));
	TriggerCollision->InitSphereRadius(300);
	TriggerCollision->SetCollisionProfileName("Trigger");
	TriggerCollision->SetupAttachment(RootComponent);

	TriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &ALootObject::OnOverlapBegin);
	TriggerCollision->OnComponentEndOverlap.AddDynamic(this, &ALootObject::OnOverlapEnd);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Object Mesh"));
	Mesh->SetupAttachment(RootComponent);

	childActorRef = CreateDefaultSubobject<UChildActorComponent>(TEXT("Child Actor"));
	childActorRef->SetupAttachment(RootComponent);

	//playerController = UGameplayStatics::GetPlayerController(UGameplayStatics::GetPlayerController(GetWorld(), 0), 0);
}

// Called when the game starts or when spawned
void ALootObject::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void ALootObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALootObject::EnableLootUI()
{
	if (LootUI && !lootUIEnabled && canOpenLoot)
	{
		lootUIEnabled = true;
		HUD = CreateWidget<UManageWidgets>(UGameplayStatics::GetPlayerController(GetWorld(), 0), LootUI);
		HUD->AddToViewport();
	}
	else if (!canOpenLoot)
	{
		moveToLootObject = true;
	}
	
}

void ALootObject::DisableLootUI(AActor* SelectedActor)
{
	if (SelectedActor)
	{
		if (*SelectedActor->GetClass()->GetSuperClass()->GetName() == FName("BP_LootObject_C") && lootUIEnabled)
		{
			HUD->RemoveFromParent();

			lootUIEnabled = false;
		}
	}
}

void ALootObject::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp && OtherComp->GetName() == "CollisionCylinder")
	{
		//UE_LOG(LogTemp, Warning, TEXT("Overlap Begin"));
		canOpenLoot = true;
		if (moveToLootObject)
		{
			EnableLootUI();
			moveToLootObject = false;
		}
	}
}

void ALootObject::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp && OtherComp->GetName() == "CollisionCylinder")
	{
		//UE_LOG(LogTemp, Warning, TEXT("Overlap End"));
		canOpenLoot = false;
	}
}

void ALootObject::GenerateRandomItems(TArray<UClass*> itemsRef)
{
	if (generateItems)
	{
		for (int i = 0; i < FMath::RandRange(0, lootLevet); i++)
		{
			for (UClass* el : itemsRef)
			{
				childActorRef->AddRelativeLocation(FVector(0, 0, -1000));
				childActorRef->SetChildActorClass(el);
				item = Cast<AMasterItem>(childActorRef->GetChildActor());
				if (item->ItemProperties.rarity > FMath::RandRange(0, 100))
				{
					AddItemToLoot(item->ItemProperties);
				}
			}
		}
	}
	else
	{
		for (FItemProperties& test : storage)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *test.name);
		}
	}
}

void ALootObject::AddItemToLoot(FItemProperties itemProperties)
{
	bool added = false;
	for (FItemProperties& storageItems : storage)
	{
		if (itemProperties.name == storageItems.name && itemProperties.isStackable && storageItems.currentAmount < storageItems.maximumAmount)
		{
			storageItems.currentAmount++;
			added = true;
		}
	}
	if (!added)
	{
		itemProperties.inInventory = false;
		itemProperties.currentAmount = 1;
		storage.Add(itemProperties);
	}
}
