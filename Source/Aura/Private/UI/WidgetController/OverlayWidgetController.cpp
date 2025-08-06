// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAuraAttributeSet()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAttributeSet()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAttributeSet()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAttributeSet()->GetMaxMana());
	
	AAuraPlayerState* AuraPS = GetAuraPlayerState();
	AuraPS->ExpChangeDelegate.Broadcast(AuraPS->GetExp());
	AuraPS->LevelChangeDelegate.Broadcast(AuraPS->GetPlayerLevel(), false);
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	//							두 가지 경우가 존재
	// 1. AuraASC에서 AbilitiesGivenDelegate가 BroadCast되기 전에 바인드 됨
	// 2. AuraASC에서 AbilitiesGivenDelegate가 BroadCast되고 난 뒤 바인드 됨
	// 1번의 경우에는 적절하지만 2번의 경우는 문제가 생긴다 이미 BroadCast된 경우 바인드 하지 않고 콜백함수를 직접 호출하는 형식으로 변경
	if (GetAuraAbilitySystemComponent()->bStartupAbilitiesGiven)
	{
		BroadcastAbilityInfo();
	}
	else
	{
		GetAuraAbilitySystemComponent()->AbilitiesGivenDelegate.AddUObject(
			this, &UOverlayWidgetController::BroadcastAbilityInfo);
	}


	// Aura Player State의 변수 Exp와 Level의 변경을 감지하여 콜백함수를 바인드
	GetAuraPlayerState()->ExpChangeDelegate.AddUObject(this, &UOverlayWidgetController::OnExpChange);
	GetAuraPlayerState()->LevelChangeDelegate.AddUObject(this, &UOverlayWidgetController::OnLevelChange);
	
	
	GetAuraAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).
		AddLambda([this](const FOnAttributeChangeData& Data) {OnHealthChanged.Broadcast(Data.NewValue); });
	GetAuraAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).
		AddLambda([this](const FOnAttributeChangeData& Data) {OnMaxHealthChanged.Broadcast(Data.NewValue); });
	GetAuraAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).
		AddLambda([this](const FOnAttributeChangeData& Data) {OnManaChanged.Broadcast(Data.NewValue); });
	GetAuraAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).
		AddLambda([this](const FOnAttributeChangeData& Data) {OnMaxManaChanged.Broadcast(Data.NewValue); });

	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				// For example, say that Tag = Message.HealthPotion 1Has a comment. Original line has a comment.
				// "Message.HealthPotion".MatchesTag("Message") will return True, 
				// "Message".MatchesTag("Message.HealthPotion") will return False
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
				if (Tag.MatchesTag(MessageTag))
				{
					FUIWidgetRow* WidgetRow = GetDataTableRowByTag<FUIWidgetRow>(MessageDataTable, Tag);
					UIWidget.Broadcast(*WidgetRow);
				}
			}
		}
	);

	//Equip 버튼을 눌러서 장착했으니 overlay도 바꿔야겠제?
	GetAuraAbilitySystemComponent()->AbilityInfoTagsDelegate.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,
	const FGameplayTag& InputSlotTag, const FGameplayTag& PrevInputSlotTag) const
{
	FAuraAbilityInfo PrevInfo;
	PrevInfo.InputTag = PrevInputSlotTag;
	PrevInfo.StatusTag = FAuraGameplayTags::Get().Abilities_Status_Unlocked;
	PrevInfo.AbilityTag = FGameplayTag();
	AbilityInfoDelegate.Broadcast(PrevInfo);

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
	Info.InputTag = InputSlotTag;
	Info.StatusTag = StatusTag;
	AbilityInfoDelegate.Broadcast(Info);
}

// Player State의 LevelInfo를 받아 현재 Exp Percent를 계산하여 UI에 BroadCast
void UOverlayWidgetController::OnExpChange(int32 Exp)
{
	ULevelUpInfo* LevelInfo = GetAuraPlayerState()->LevelUpInfo;
	check(LevelInfo);

	const int32 CurrentLevel = LevelInfo->GetLevelByExpAmount(Exp);
	const int32 MaxLevel = LevelInfo->LevelUpInfos.Num()-1;

	if (CurrentLevel <= MaxLevel && CurrentLevel > 0)
	{
		int32 CurrentLevelExpAmount = LevelInfo->LevelUpInfos[CurrentLevel].LevelUpExpAmount;
		int32 PrevLevelExpAmount = LevelInfo->LevelUpInfos[CurrentLevel-1].LevelUpExpAmount;
		int32 DeltaExpAmount = CurrentLevelExpAmount - PrevLevelExpAmount;

		int32 CurrentDeltaExpAmount = Exp - PrevLevelExpAmount;
		const float ExpPercent = static_cast<float>(CurrentDeltaExpAmount) / static_cast<float>(DeltaExpAmount);

		ExpChangeDelegate.Broadcast(ExpPercent);
	}

}
void UOverlayWidgetController::OnLevelChange(int32 Level, bool bIsLevelUp)
{
	LevelChangeDelegate.Broadcast(Level, bIsLevelUp);
}