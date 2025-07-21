// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerState.h"

//메뉴 위젯이 만들어질 때 초기값 설정 블루프린트에서 사용
void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	check(AttributeInfo);
	for (auto& Pair : GetAuraAttributeSet()->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	PlayerAttributePointChangedDelegate.Broadcast(GetAuraPlayerState()->GetAttributePoint());
}

//값이 바뀔 때 마다 어떻게 값을 넘겨주는가를 초기에 설정 / AuraHUD에서 사용
void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	check(AttributeInfo);

	for (auto& Pair : GetAuraAttributeSet()->TagsToAttributes)
	{
		GetAuraAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}


	GetAuraPlayerState()->AttributePointChangeDelegate.AddLambda([this](int32 AttributePoint)
	{
		PlayerAttributePointChangedDelegate.Broadcast(AttributePoint);
	});
}

void UAttributeMenuWidgetController::UpgradeAttributes(const FGameplayTag& Tag)
{
	// 이 로직을 왜 ASC에서 할까 ? --> GE 적용이나 GameplayEvent 전송은 ASC에서 함
	GetAuraAbilitySystemComponent()->UpgradeAttributes(Tag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
