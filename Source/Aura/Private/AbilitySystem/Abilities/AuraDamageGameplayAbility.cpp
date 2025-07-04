// Copyright Druid Mechanics


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"



// 데미지 계산 및 적용을 위한 공통 로직
// CauseDamage 함수: 지정된 대상 액터에게 실제 데미지를 적용하는 로직
void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
    FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f); // 현재 레벨은 1로 하드코딩

    // 모든 피해 유형에 대해 반복하며 데미지 값을 GE Spec에 주입
    //    DamageTypes 맵은 FGameplayTag (피해 유형, 예: Physical, Fire)와 FScalableFloat (레벨별 피해량 곡선) 쌍으로 구성
    for (TTuple<FGameplayTag, FScalableFloat> Pair : DamageTypes)
    {
       //현재 어빌리티 레벨에 따른 피해량 계산
       const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());

       //계산된 피해량을 GE Spec에 'Set By Caller Magnitude'로 주입:
       //    AssignTagSetByCallerMagnitude: GE Spec에 특정 GameplayTag (키)와 그에 해당하는 float 값 (피해량)을 할당합니다.
       //    DamageSpecHandle: 값을 주입할 대상 GE Spec 핸들
       //    Pair.Key: 현재 반복 중인 피해 유형의 GameplayTag
       //    ScaledDamage: 계산된 최종 피해량.
       //    -> 이렇게 주입된 정보는 이후 Execution Calculation (EC)에서 특정 태그로 조회하여 데미지 계산에 활용
       UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, ScaledDamage);
    }
    
    // 3. 구성된 Gameplay Effect Spec을 대상 액터에게 적용:
    //    -> 이 호출로 인해 DamageSpecHandle에 담긴 데미지 정보가 TargetActor의 ASC로 전달되고,
    //       연결된 Execution Calculation이 서버에서 실행되어 최종 데미지 계산 및 적용이 이루어집니다.
    GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}