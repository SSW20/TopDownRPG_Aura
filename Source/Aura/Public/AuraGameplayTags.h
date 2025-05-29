// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 싱글톤 패턴을 사용하여 네이티브 게임플레이 태그를 포함하는 구조체
 *
 * C++ 코드에서 게임플레이 태그를 변수 형태로 쉽게 접근하고 사용할 수 있도록 중앙 집중식으로 관리합니다.
 */
 
struct FAuraGameplayTags
{
public:
	static const FAuraGameplayTags& Get() { return GameplayTags; }
	static void InitializeGameplayTags();
	
	//	Primary Attributes
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;


	// Secondary Attributes
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CritHitChance;
	FGameplayTag Attributes_Secondary_CritHitDamage;
	FGameplayTag Attributes_Secondary_CritHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegenerate;
	FGameplayTag Attributes_Secondary_ManaRegenerate;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;


	
protected:
private:
	static FAuraGameplayTags GameplayTags;
};

