// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AuraWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/Data/AbilityInfo.h"

UAuraAttributeSet* UAuraWidgetController::GetAuraAttributeSet()
{
	if (AuraAttributeSet == nullptr)
	{
		AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	}
	return AuraAttributeSet;
}

UAuraAbilitySystemComponent* UAuraWidgetController::GetAuraAbilitySystemComponent()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	}
	return AuraAbilitySystemComponent;
}

AAuraPlayerState* UAuraWidgetController::GetAuraPlayerState()
{
	if (AuraPlayerState == nullptr)
	{
		AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	}
	return AuraPlayerState;
}

AAuraPlayerController* UAuraWidgetController::GetAuraPlayerController()
{
	if (AuraPlayerController == nullptr)
	{
		AuraPlayerController = CastChecked<AAuraPlayerController>(PlayerController);
	}
	return AuraPlayerController;
}

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValues()
{
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
}


// ASC를 통해 모든 GA를 본 뒤 InputTag를 가져와 AbilityInfo 데이터 에셋에서 정보를 추가하여 블루프린트에 BroadCast
void UAuraWidgetController::BroadcastAbilityInfo()
{
	if (!GetAuraAbilitySystemComponent()->bIsStartUpAbilitiesBroadCasted) return;
	
	// FForEachAbility 델리게이트 인스턴스를 생성
	// 이 델리게이트는 UAuraAbilitySystemComponent::ForEachAbility 함수에 전달되어 각 활성화 가능한 어빌리티 스펙에 대해 실행될 콜백을 정의.
	FForEachAbility ForEachDelegate;
	ForEachDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(GetAuraAbilitySystemComponent()->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = GetAuraAbilitySystemComponent()->GetInputTagFromSpec(AbilitySpec);
		Info.StatusTag = GetAuraAbilitySystemComponent()->GetStatusTagFromSpec(AbilitySpec);

		//완성된 FAuraAbilityInfo 정보를 AbilityInfoDelegate를 통해 블루프린트(UI)에 브로드캐스트하여 어빌리티 정보를 전달
		AbilityInfoDelegate.Broadcast(Info);
	});

	// ASC의 ForEachAbility 함수를 호출하여, 현재 활성화 가능한 모든 어빌리티에 대해
	// 위에서 정의한 ForEachDelegate 람다를 실행하도록 지시
	GetAuraAbilitySystemComponent()->ForEachAbility(ForEachDelegate);
}
