// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterItem.h"

// Sets default values
AMasterItem::AMasterItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("OnClicked_Component");
	SphereComponent->InitSphereRadius(100);

	SphereComponent->OnClicked.AddDynamic(this, &AMasterItem::OnSelected);

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


AMasterItem* AMasterItem::ItemReference()
{
	return this;
}

void AMasterItem::DestroyObject()
{
	Destroy();
}
