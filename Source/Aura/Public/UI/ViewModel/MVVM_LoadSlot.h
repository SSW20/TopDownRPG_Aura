// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "MVVM_LoadSlot.generated.h"
/**
 * 
 */
 

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetSwitcherIndex, int32, SwitcherIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FButtonEnable, bool, bEnable);

UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FSetSwitcherIndex SetSwitcherIndex;

	UPROPERTY(BlueprintAssignable)
	FButtonEnable SetButtonEnable;
	
	UPROPERTY()
	int32 SlotIndex = 0;
	
	void UpdateSlotData();
	
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SlotStatus;
	
	void SetPlayerName(const FString& InPlayerName);
	void SetSlotName(const FString& InSlotName);
	void SetMapName(const FString& InMapName);
	
	FString GetPlayerName() const {return PlayerName;}
	FString GetSlotName() const {return SlotName;}
	FString GetMapName() const {return MapName;}
private:
	/*	Field Notify */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess = "true"))
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess = "true"))
	FString SlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess = "true"))
	FString MapName;
};
