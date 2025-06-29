// Copyright Druid Mechanics
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "AuraAbilitySystemGlobals.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UAuraAbilitySystemGlobals : public UAbilitySystemGlobals
{
	// 핵심: FGameplayEffectContext를 생성할 때 호출되는 가상 함수를 오버라이드
	// 이 함수가 반환하는 타입이 프로젝트에서 사용될 기본 FGameplayEffectContext 타입이 됩니다
	GENERATED_BODY()
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};