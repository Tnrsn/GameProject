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

		HUD = CreateWidget<UManageWidgets>(UGameplayStatics::GetPlayerController(GetWorld(), 0), LootUI);
		HUD->AddToViewport();

		lootUIEnabled = true;

	}
	
}

void ALootObject::DisableLootUI(AActor* SelectedActor)
{
	if (SelectedActor)
	{
		if (*SelectedActor->GetClass()->GetSuperClass()->GetName() == FName("BP_LootObject_C") && lootUIEnabled)
		{
			HUD->RemoveFromViewport();
			lootUIEnabled = false;
		}
	}
}

void ALootObject::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap Begin"));
		canOpenLoot = true;
	}
}

void ALootObject::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap End"));
		canOpenLoot = false;
	}
}

void ALootObject::GenerateRandomItems(TArray<UClass*> itemsRef)
{
	for (int i = 0; i < FMath::RandRange(0, lootLevet); i++)
	{
		for (UClass* el : itemsRef)
		{
			bool added = false;
			childActorRef->SetChildActorClass(el);
			item = Cast<AMasterItem>(childActorRef->GetChildActor());
			if (item->ItemProperties.rarity > FMath::RandRange(0, 100))
			{
				for (FItemProperties& storageItems : storage)
				{
					if (item->ItemProperties.name == storageItems.name && item->ItemProperties.isStackable)
					{
						storageItems.currentAmount++;
						added = true;
					}
				}
				if (!added)
				{
					storage.Add(item->ItemProperties);
				}
			}
		}
	}
}
