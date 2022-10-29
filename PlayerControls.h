// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <TimerManager.h>
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <Camera/CameraComponent.h>
#include <Components/BoxComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <DrawDebugHelpers.h>
#include <GameFramework/PlayerController.h>
#include "GameFramework/Character.h"
#include "PlayerControls.generated.h"

UCLASS()
class GAMEPROJECT_API APlayerControls : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerControls();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* Camera;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void MoveForward(float value);
	void MoveRight(float value);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
