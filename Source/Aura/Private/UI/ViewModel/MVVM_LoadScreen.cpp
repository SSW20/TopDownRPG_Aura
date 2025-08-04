// Copyright Druid Mechanics


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(GetWorld(), LoadSloatViewModelClass);
	LoadSlot_0->SetSlotName(FString("LoadSlot_0"));
	LoadSlots.Add(0,LoadSlot_0);

	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(GetWorld(), LoadSloatViewModelClass);
	LoadSlot_1->SetSlotName(FString("LoadSlot_1"));
	LoadSlots.Add(1,LoadSlot_1);

	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(GetWorld(), LoadSloatViewModelClass);
	LoadSlot_2->SetSlotName(FString("LoadSlot_2"));
	LoadSlots.Add(2,LoadSlot_2);

	SetNumLoadSlots(LoadSlots.Num());
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& NewName)
{
	AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (AuraGameModeBase)
	{
		LoadSlots[Slot]->SetPlayerName(NewName);
		AuraGameModeBase->SaveSlotData(LoadSlots[Slot], Slot);
	}
	LoadSlots[Slot]->SetSwitcherIndex.Broadcast(2);
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->SetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::LoadButtonPressed(int32 Slot)
{
}

void UMVVM_LoadScreen::SetNumLoadSlots(int32 InNumSlots)
{
	UE_MVVM_SET_PROPERTY_VALUE(NumLoadSlots, InNumSlots);
}
