// Copyright Druid Mechanics


#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::UpdateSlotData()
{
	const int32 WidgetSwitcherIndex = SlotStatus.GetValue();
	SetSwitcherIndex.Broadcast(WidgetSwitcherIndex);
}

void UMVVM_LoadSlot::SetPlayerName(const FString& InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}

void UMVVM_LoadSlot::SetSlotName(const FString& InSlotName)
{
	UE_MVVM_SET_PROPERTY_VALUE(SlotName, InSlotName);
}

void UMVVM_LoadSlot::SetMapName(const FString& InMapName)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName, InMapName);
}
void UMVVM_LoadSlot::SetPlayerLevel(int32 InPlayerLevel)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerLevel, InPlayerLevel);
}
