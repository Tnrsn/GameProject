// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestSystem.h"

UQuestSystem::UQuestSystem()
{
	mainQuestLine[0] = true;

	for (int i = 1; i < 5; i++)
	{
		mainQuestLine[i] = false;
	}
}
