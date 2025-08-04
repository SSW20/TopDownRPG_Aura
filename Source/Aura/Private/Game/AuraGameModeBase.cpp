// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AuraGameModeBase.h"

#include "Game/LoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"

void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlot->SlotName, SlotIndex);
	}
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);

	LoadScreenSaveGame->SlotName = LoadSlot->SlotName;
	LoadScreenSaveGame->SlotIndex = SlotIndex;
	LoadScreenSaveGame->PlayerName = LoadSlot->PlayerName;

	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame,LoadSlot->SlotName, SlotIndex);
}
