// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCDialogSystem.h"

UNPCDialogSystem::UNPCDialogSystem()
{

}

void UNPCDialogSystem::EnableDialogUI(TSubclassOf<UUserWidget> dialogBoxUI, AActor* actor)
{
	dialogBox = CreateWidget<UManageWidgets>(UGameplayStatics::GetPlayerController(actor->GetWorld(), 0), dialogBoxUI);

	if (dialogBox)
	{
		dialogBox->AddToViewport();
	}
}

void UNPCDialogSystem::DisableDialogUI()
{
	if (dialogBox)
	{
		dialogBox->RemoveFromParent();
	}
}

void UNPCDialogSystem::RefreshDialogUI(TSubclassOf<UUserWidget> dialogBoxUI, AActor* actor)
{
	DisableDialogUI();
	EnableDialogUI(dialogBoxUI, actor);
}
