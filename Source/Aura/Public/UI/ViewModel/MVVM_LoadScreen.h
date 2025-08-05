// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadSlot.h"
#include "MVVM_LoadScreen.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnableStartAndDelete);

UCLASS()
class AURA_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FEnableStartAndDelete OnSelectButtonClicked;
	
	void InitializeLoadSlots();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot> LoadSloatViewModelClass;

	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot* GetLoadSlotViewModelByIndex(int32 Index) const;

	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 Slot, const FString& NewName);

	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void SelectButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void PlayButtonPressed();

	UFUNCTION(BlueprintCallable)
	void DeleteButtonPressed();

	void LoadData();
	
	void SetNumLoadSlots(int32 InNumSlots);
	int32 GetNumLoadSlots() const {return NumLoadSlots;}
private:
	UPROPERTY()
	TMap<int32, UMVVM_LoadSlot*> LoadSlots;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_0;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_1;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess = "true"));
	int32 NumLoadSlots;

	UMVVM_LoadSlot* SelectedSlot;
};
