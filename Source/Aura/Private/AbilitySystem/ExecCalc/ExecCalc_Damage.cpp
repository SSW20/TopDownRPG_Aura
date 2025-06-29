// Copyright Druid Mechanics


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"


UExecCalc_Damage::UExecCalc_Damage()
{
	// RelevantAttributesToCapture 배열에 추가된 능력치들만 ExecutionParams.AttemptCalculateCapturedAttributeMagnitude를 통해 캡처할 수 있음
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritHitResistDef);

}


void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// ExecutionParams: GE 실행에 필요한 모든 입력 정보 (소스, 타겟, GE 스펙 등)를 담고 있는 구조체.
	// OutExecutionOutput: 계산된 최종 결과를 GAS 시스템에 전달하여 능력치를 실제로 변경하는 데 사용되는 출력 구조체

	// ExecutionParams을 통해 ASC를 가져옴 
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	// ASC를 가져오면 ASC를 가지는 액터를 가져올 수 있음
	AActor* SourcActor =  SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetActor = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	//ExecutionParams을 통해 GameplayEffectSpec(GE의 설계도)을 가져옴 
	const FGameplayEffectSpec EffectSpec = ExecutionParams.GetOwningSpec();

	//EffectSpec에서 Source와 Target에 붙어 있는 Gameplay Tag들을 가져옴
	const FGameplayTagContainer* SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

	//능력치 평가에 필요한 파라미터 구조체인 FAggregatorEvaluateParameters를 초기화
	//능력치 값을 평가할 때 소스/타겟 태그의 영향을 고려
	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = SourceTags;
	EvaluateParams.TargetTags = TargetTags;

	FGameplayEffectContextHandle EffectContextHandle = EffectSpec.GetContext();


	/*
	 *		Damage 계산 로직 부분
	 */
	
	//Damage를 가져옴
	float Damage = 0.f;
	Damage = EffectSpec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);

	//Source의 CritHitChance 속성을 캡쳐하여 SourceCritHitChance 값을 변경 
	float SourceCritHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritHitChanceDef, EvaluateParams, SourceCritHitChance);
	SourceCritHitChance = FMath::Max<float>(0.f, SourceCritHitChance);

	//Target의 CritHitResist 속성을 캡쳐하여 TargetCritHitResist 값을 변경 
	float TargetCritHitResist = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritHitResistDef, EvaluateParams, TargetCritHitResist);
	TargetCritHitResist = FMath::Max<float>(0.f, TargetCritHitResist);
	
	//Source의 CritHitDamage 속성을 캡쳐하여 SourceCritHitDamage 값을 변경 (고정 방관)
	float SourceCritHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritHitDamageDef, EvaluateParams, SourceCritHitDamage);
	
	// 크리 터졌을 시 붕스 식 크뎀 적용
	float TargetCritHitChance = SourceCritHitChance - TargetCritHitResist;
	bool bIsCrit = FMath::RandRange(0.f, 100.f) < TargetCritHitChance;
	if (bIsCrit) Damage *= (1.f + SourceCritHitDamage / 100.f) ;

	// bCrit값 설정
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bIsCrit);
	
	//Target의 BlockChance 속성을 캡쳐하여 TargetBlockChance 값을 변경
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluateParams, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(0.f, TargetBlockChance);
	
	// 막았을 시 데미지 반감
	bool bBlocked = FMath::RandRange(0, 100) < TargetBlockChance;
	if (bBlocked) Damage /= 2.f;

	// bBlock 값 설정 
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);

	//Target의 Armor 속성을 캡쳐하여 TargetArmor 값을 변경
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParams, TargetArmor);
	TargetArmor = FMath::Max<float>(0.f, TargetArmor);

	//Source의 ArmorPenetration 속성을 캡쳐하여 SourceArmorPenetration 값을 변경 (고정 방관)
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluateParams, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(0.f, SourceArmorPenetration);

	// Armor는 퍼센트로 감소
	TargetArmor = FMath::Max(0,TargetArmor - SourceArmorPenetration);
	Damage *= (100 - TargetArmor) / 100.f;

	
	//계산된 결과를 어떻게 적용할지 설정  
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	
	//OutExecutionOutput은 자동으로 Target의 AbilitySystemComponent에 대한 변경 지시를 담는 '컨테이너' 역할을 함
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
	
}
