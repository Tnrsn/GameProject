// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterItem.h"

// Sets default values
AMasterItem::AMasterItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//SphereComponent->OnClicked.AddDynamic(this, &AMasterItem::OnSelected);
	TriggerCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger Sphere"));
	TriggerCollision->InitSphereRadius(300);
	TriggerCollision->SetCollisionProfileName("Trigger");
	RootComponent = TriggerCollision;

	TriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &AMasterItem::OnOverlapBegin);
	TriggerCollision->OnComponentEndOverlap.AddDynamic(this, &AMasterItem::OnOverlapEnd);

	SphereComponent = CreateDefaultSubobject<USphereComponent>("OnClicked_Component");
	SphereComponent->InitSphereRadius(100);
	SphereComponent->SetupAttachment(TriggerCollision);
}

// Called when the game starts or when spawned
void AMasterItem::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMasterItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AMasterItem::OnSelected(UPrimitiveComponent* PrimComp, FKey InKey)
{
	/*UE_LOG(LogTemp, Warning, TEXT("Test"));*/
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *GetActorLocation().ToString());
	
}

void AMasterItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap Begin"));
		canLoot = true;

	}
}

void AMasterItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap End"));
		canLoot = false;
	}
}
