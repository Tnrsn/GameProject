// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ManageWidgets.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UManageWidgets : public UUserWidget
{
	GENERATED_BODY()

public:
	UManageWidgets(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable)
		void ButtonPressed();
	UFUNCTION(BlueprintCallable)
		void ButtonReleased();

	UPROPERTY(BlueprintReadWrite)
		int32 testNum;
};
