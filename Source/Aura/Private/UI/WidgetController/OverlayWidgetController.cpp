// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
	
	AAuraPlayerState* AuraPS = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPS->ExpChangeDelegate.Broadcast(AuraPS->GetExp());
	AuraPS->LevelChangeDelegate.Broadcast(AuraPS->GetPlayerLevel());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		//							두 가지 경우가 존재
		// 1. AuraASC에서 AbilitiesGivenDelegate가 BroadCast되기 전에 바인드 됨
		// 2. AuraASC에서 AbilitiesGivenDelegate가 BroadCast되고 난 뒤 바인드 됨
		// 1번의 경우에는 적절하지만 2번의 경우는 문제가 생긴다 이미 BroadCast된 경우 바인드 하지 않고 콜백함수를 직접 호출하는 형식으로 변경
		if (AuraASC->bIsStartUpAbilitiesBroadCasted)
		{
			BindStartupAbilities(AuraASC);
		}
		else
		{
			AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BindStartupAbilities);
		}
	}

	AAuraPlayerState* AuraPS = CastChecked<AAuraPlayerState>(PlayerState);
	if (AuraPS)
	{
		// Aura Player State의 변수 Exp와 Level의 변경을 감지하여 콜백함수를 바인드
		AuraPS->ExpChangeDelegate.AddUObject(this, &UOverlayWidgetController::OnExpChange);
		AuraPS->LevelChangeDelegate.AddUObject(this, &UOverlayWidgetController::OnLevelChange);
	}
	
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).
		AddLambda([this](const FOnAttributeChangeData& Data) {OnHealthChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).
		AddLambda([this](const FOnAttributeChangeData& Data) {OnMaxHealthChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).
		AddLambda([this](const FOnAttributeChangeData& Data) {OnManaChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).
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
}

// ASC를 통해 모든 GA를 본 뒤 InputTag를 가져와 AbilityInfo 데이터 에셋에서 정보를 추가하여 블루프린트에 BroadCast
void UOverlayWidgetController::BindStartupAbilities(UAuraAbilitySystemComponent* ASC)
{
	if (!ASC->bIsStartUpAbilitiesBroadCasted) return;
	
	// FForEachAbility 델리게이트 인스턴스를 생성
	// 이 델리게이트는 UAuraAbilitySystemComponent::ForEachAbility 함수에 전달되어 각 활성화 가능한 어빌리티 스펙에 대해 실행될 콜백을 정의.
	FForEachAbility ForEachDelegate;
	ForEachDelegate.BindLambda([this, ASC](const FGameplayAbilitySpec& AbilitySpec)
	{
		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(ASC->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = ASC->GetInputTagFromSpec(AbilitySpec);

		//완성된 FAuraAbilityInfo 정보를 AbilityInfoDelegate를 통해 블루프린트(UI)에 브로드캐스트하여 어빌리티 정보를 전달합니다.
		AbilityInfoDelegate.Broadcast(Info);
	});

	// ASC의 ForEachAbility 함수를 호출하여, 현재 활성화 가능한 모든 어빌리티에 대해
	// 위에서 정의한 ForEachDelegate 람다를 실행하도록 지시합니다.
	ASC->ForEachAbility(ForEachDelegate);
}

// Player State의 LevelInfo를 받아 현재 Exp Percent를 계산하여 UI에 BroadCast
void UOverlayWidgetController::OnExpChange(int32 Exp)
{
	const AAuraPlayerState* AuraPS = CastChecked<AAuraPlayerState>(PlayerState);
	ULevelUpInfo* LevelInfo = AuraPS->LevelUpInfo;
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
void UOverlayWidgetController::OnLevelChange(int32 Level)
{
	LevelChangeDelegate.Broadcast(Level);
}