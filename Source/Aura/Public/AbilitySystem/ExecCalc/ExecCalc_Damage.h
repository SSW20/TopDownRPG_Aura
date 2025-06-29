// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AuraGameplayTags.h"
#include "ExecCalc_Damage.generated.h"

/**
 *
 */

struct FAuraGameplayTags;

struct AuraDamageStatics
{
	//Armor의 FProperty*와 FGameplayEffectAttributeCaptureDefinition 변수를 선언
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritHitResist);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCapturesMap;

	AuraDamageStatics()
	{
		// 선언된 능력치 캡처 정의를 구체적으로 초기화합니다.
		// S: 해당 능력치가 속한 AttributeSet 클래스 
		// P: 능력치 이름 
		// T: 능력치를 누구로부터 캡처할지 (Target - 피해자로부터 가져옴)
		// B: 스냅샷(Snapshot) 여부 (false - 즉시 적용 시점의 값을 가져옴)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CritHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CritHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CritHitResist, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);

		const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
		TagsToCapturesMap.Add(Tags.Attributes_Secondary_Armor, ArmorDef);
		TagsToCapturesMap.Add(Tags.Attributes_Secondary_BlockChance, BlockChanceDef);
		TagsToCapturesMap.Add(Tags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
		TagsToCapturesMap.Add(Tags.Attributes_Secondary_CritHitChance, CritHitChanceDef);
		TagsToCapturesMap.Add(Tags.Attributes_Secondary_CritHitResistance, CritHitResistDef);
		TagsToCapturesMap.Add(Tags.Attributes_Secondary_CritHitDamage, CritHitDamageDef);

		TagsToCapturesMap.Add(Tags.Attributes_Resistance_Arcane, ArcaneResistanceDef);
		TagsToCapturesMap.Add(Tags.Attributes_Resistance_Fire, FireResistanceDef);
		TagsToCapturesMap.Add(Tags.Attributes_Resistance_Lightning, LightningResistanceDef);
		TagsToCapturesMap.Add(Tags.Attributes_Resistance_Physical, PhysicalResistanceDef);
	}
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DamageStatics;
	return DamageStatics;
}

UCLASS()
class AURA_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UExecCalc_Damage();

	// 이 함수는 Execution Calculation의 핵심으로, Gameplay Effect가 적용될 때 서버에서 실행
	// PostGameplayEffectExecute 함수보다 먼저(이전) 실행
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
