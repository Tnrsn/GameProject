// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCDialogSystem.h"

UNPCDialogSystem::UNPCDialogSystem()
{

}

void UNPCDialogSystem::EnableDialogBox(TSubclassOf<UUserWidget> dialogBoxUI, AActor* actor)
{
	dialogBox = CreateWidget<UManageWidgets>(UGameplayStatics::GetPlayerController(actor->GetWorld(), 0), dialogBoxUI);
	if (dialogBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("test"));
		dialogBox->AddToViewport();
	}
}

void UNPCDialogSystem::DisableDialogBox()
{
	if (dialogBox)
	{
		dialogBox->RemoveFromParent();
	}
}

