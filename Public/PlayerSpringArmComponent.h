// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerSpringArmComponent.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UPlayerSpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()
	
public:
	float mouseSensitivity = 0.3f;
	bool rotationStarted = false;

	FVector2D mousePosition;
	FVector2D mouseDelta;
	FVector2D previousMousePosition;

	//Functions
	void RotateCamera(APlayerController* playerController);
	void LookFromFront();
};
