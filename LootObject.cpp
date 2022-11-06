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


	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Object Mesh"));
	Mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ALootObject::BeginPlay()
{
	Super::BeginPlay();

	Mesh->SetupAttachment(RootComponent);
}


// Called every frame
void ALootObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALootObject::GetType()
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *type.ToString());
}