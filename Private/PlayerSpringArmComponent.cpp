// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSpringArmComponent.h"

void UPlayerSpringArmComponent::RotateCamera(APlayerController* playerController)
{
	playerController->GetMousePosition(mousePosition.X, mousePosition.Y);

	if (rotationStarted)
	{
		previousMousePosition = mousePosition;
		rotationStarted = false;
	}

	mouseDelta = mousePosition - previousMousePosition;

	FRotator NewRotation = this->GetComponentRotation();
	NewRotation.Yaw += mouseDelta.X * mouseSensitivity;
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch - mouseDelta.Y * mouseSensitivity, -70.0f, 30.0f); //Camera rotation limits 

	this->SetWorldRotation(NewRotation);

	previousMousePosition = mousePosition;
}

void UPlayerSpringArmComponent::LookFromFront()
{
	bEnableCameraRotationLag = false;

	SetRelativeRotation(FRotator(-5, 170, 0));

	TargetArmLength = 150.f;
	TargetOffset = FVector3d(0, 55, -20);
}
