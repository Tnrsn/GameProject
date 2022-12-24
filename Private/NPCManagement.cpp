// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCManagement.h"

// Sets default values
ANPCManagement::ANPCManagement()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	capsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Sphere Collision"));
	capsuleComponent->InitCapsuleSize(41, 92);
	RootComponent = capsuleComponent;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("NPC Mesh"));
	SkeletalMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ANPCManagement::BeginPlay()
{
	Super::BeginPlay();
	
	dialogSystem = NewObject<UNPCDialogSystem>();
}

// Called every frame
void ANPCManagement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANPCManagement::StartDialog()
{
	dialogSystem->conversation = Conversations;
	dialogSystem->EnableDialogUI(dialogBoxUI, this);
}

