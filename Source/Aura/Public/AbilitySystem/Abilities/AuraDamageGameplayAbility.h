// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayAbility.h"
#include "GameplayEffect.h"
#include "Interaction/CombatInterface.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamgeEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;

	UFUNCTION(BlueprintPure)
	float GetDamageAtLevel() const;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> DamageEffectClass; // 사용할 데미지 GE 클래스
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	FGameplayTag DamageTypeTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	FScalableFloat Damage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffChance = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffDamage = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffFrequency = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffDuration = 5.f;

	// 죽었을 때 날라가는 정도
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DeathImpulseMagnitude = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float KnockbackMagnitude = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float KnockbackChance = 20.f;
	
	UFUNCTION(BlueprintPure)
	FTaggedMontage GetTaggedMontageInArray(const TArray<FTaggedMontage>& TaggedMontages);

};
