// Copyright Druid Mechanics
#include "AbilitySystem/AuraAbilitySystemGlobals.h"

#include "AuraAbilityTypes.h"

FGameplayEffectContext* UAuraAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	// 새로운 FAuraGameplayEffectContext 객체를 생성하여 반환합니다.
	// 이제 이 프로젝트에서 모든 AllocGameplayEffectContext() 호출은 이 함수를 통하게 되므로,
	// 우리가 만든 FAuraGameplayEffectContext가 사용됩니다.
	return new FAuraGameplayEffectContext();
}