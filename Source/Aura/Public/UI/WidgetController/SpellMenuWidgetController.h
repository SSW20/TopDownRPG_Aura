// Copyright Druid Mechanics

#pragma once
#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "GameplayTagContainer.h"
#include "SpellMenuWidgetController.generated.h"
/**
 * 
 */
struct FSelectedAbility
{
	FGameplayTag Ability = FGameplayTag();
	FGameplayTag Status = FGameplayTag();
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellGlobeSelectedSignature, bool, bSpendPointsButtonEnabled, bool, bEquipButtonEnabled, FString, DescriptionString, FString, NextLevelDescriptionString);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityTypeSignature, FGameplayTag, AbiltyTypeTag);

UCLASS()
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	
	UPROPERTY(BlueprintAssignable, Category = "GAS|PlayerState")
	FOnPlayerPointChanged PlayerSkillPointChangedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;

	UPROPERTY(BlueprintAssignable)
	FAbilityTypeSignature WaitEquipSelectDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FAbilityTypeSignature StopWaitEquipSelectDelegate;

	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	void PressedSpellPointButton();

	UFUNCTION(BlueprintCallable)
	void DeselectGlobe();

	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed();
	
	UFUNCTION(BlueprintCallable)
    void EquipGlobePressed(const FGameplayTag& SlotInputTag, const FGameplayTag& AbilityType);

	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,
	const FGameplayTag& InputSlotTag, const FGameplayTag& PrevInputSlotTag);
private:
	void ShouldEnableButtons(const FGameplayTag& Statustag, int32 SpellPoints, bool& bOutSpellPointsButton, bool& bOutEquipButton);

	FSelectedAbility SelectedAbility = FSelectedAbility();
	int32 CurrentSpellPoints = 0;

	bool bWaitingForEquip = false;

	FGameplayTag SelectedSlot = FGameplayTag();
};
