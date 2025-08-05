// Copyright Druid Mechanics


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSloatViewModelClass);
	LoadSlot_0->SetSlotName(FString("LoadSlot_0"));
	LoadSlots.Add(0,LoadSlot_0);

	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSloatViewModelClass);
	LoadSlot_1->SetSlotName(FString("LoadSlot_1"));
	LoadSlots.Add(1,LoadSlot_1);

	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSloatViewModelClass);
	LoadSlot_2->SetSlotName(FString("LoadSlot_2"));
	LoadSlots.Add(2,LoadSlot_2);

	SetNumLoadSlots(LoadSlots.Num());
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

// 새로운 데이터를 받아 저장슬롯을 만듦
void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& NewName)
{
	AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (AuraGameModeBase)
	{
		LoadSlots[Slot]->SetPlayerName(NewName);
		LoadSlots[Slot]->SetMapName(AuraGameModeBase->DefaultMapName);
		LoadSlots[Slot]->SlotStatus = Taken;

		AuraGameModeBase->SaveSlotData(LoadSlots[Slot], Slot);
	}
	LoadSlots[Slot]->SetSwitcherIndex.Broadcast(LoadSlots[Slot]->SlotStatus);
}

// 빈 슬롯에 이제 저장할 내용을 불러옴
void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->SetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::SelectButtonPressed(int32 Slot)
{
	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		if (LoadSlot.Key == Slot)
		{
			LoadSlot.Value->SetButtonEnable.Broadcast(false);
		}
		else
		{
			LoadSlot.Value->SetButtonEnable.Broadcast(true);
		}
	}
	OnSelectButtonClicked.Broadcast();
	SelectedSlot = LoadSlots[Slot];
}

// 실제로 게임 실행
void UMVVM_LoadScreen::PlayButtonPressed()
{
	if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		AuraGameMode->TravelMap(SelectedSlot);
	}
}

void UMVVM_LoadScreen::DeleteButtonPressed()
{
	if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		AuraGameMode->DeleteSlot(SelectedSlot->GetSlotName(), SelectedSlot->SlotIndex);
		SelectedSlot->SlotStatus = Vacant;
		SelectedSlot->UpdateSlotData();
		SelectedSlot->SetButtonEnable.Broadcast(true);
	}
}

void UMVVM_LoadScreen::LoadData()
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		ULoadScreenSaveGame* SaveObject = AuraGameMode->GetSaveSlotData(LoadSlot.Value->GetSlotName(), LoadSlot.Key);

		const FString PlayerName = SaveObject->PlayerName;
		const FString MapName = SaveObject->MapName;
		TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = SaveObject->SaveSlotStatus;

		LoadSlot.Value->SlotStatus = SaveSlotStatus;
		LoadSlot.Value->SetPlayerName(PlayerName);
		LoadSlot.Value->SetMapName(MapName);
		LoadSlot.Value->UpdateSlotData();
	}
}

void UMVVM_LoadScreen::SetNumLoadSlots(int32 InNumSlots)
{
	UE_MVVM_SET_PROPERTY_VALUE(NumLoadSlots, InNumSlots);
}
