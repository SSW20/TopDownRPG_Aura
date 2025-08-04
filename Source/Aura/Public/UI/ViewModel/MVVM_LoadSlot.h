// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadSlot.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetSwitcherIndex, int32, SwitcherIndex);

UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FSetSwitcherIndex SetSwitcherIndex;

	UPROPERTY()
	int32 SlotIndex = 0;


	/*	Field Notify */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess = "true"))
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess = "true"))
	FString SlotName;
	
	void SetPlayerName(const FString& InPlayerName);
	void SetSlotName(const FString& InSlotName);
	
	FString GetPlayerName() const {return PlayerName;}
	FString GetSlotName() const {return SlotName;}
	
};
