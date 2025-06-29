// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "GameplayEffect.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> DamageEffectClass; // 사용할 데미지 GE 클래스
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	TMap<FGameplayTag,FScalableFloat> DamageTypes;
};
