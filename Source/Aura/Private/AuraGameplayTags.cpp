// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeGameplayTags()
{
	UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();

	// Primary Attributes
	GameplayTags.Attributes_Primary_Strength = TagManager.AddNativeGameplayTag(FName("Attributes.Primary.Strength"), FString("주요 능력치: 힘"));
	GameplayTags.Attributes_Primary_Intelligence = TagManager.AddNativeGameplayTag(FName("Attributes.Primary.Intelligence"), FString("주요 능력치: 지능"));
	GameplayTags.Attributes_Primary_Resilience = TagManager.AddNativeGameplayTag(FName("Attributes.Primary.Resilience"), FString("주요 능력치: 회복력"));
	GameplayTags.Attributes_Primary_Vigor = TagManager.AddNativeGameplayTag(FName("Attributes.Primary.Vigor"), FString("주요 능력치: 활력"));

	// Secondary Attributes
	GameplayTags.Attributes_Secondary_Armor = TagManager.AddNativeGameplayTag(FName("Attributes.Secondary.Armor"), FString("보조 능력치: 방어력, 받는 피해 감소, 막기 확률 증가"));
	GameplayTags.Attributes_Secondary_ArmorPenetration = TagManager.AddNativeGameplayTag(FName("Attributes.Secondary.ArmorPenetration"), FString("보조 능력치: 방어 관통"));
	GameplayTags.Attributes_Secondary_BlockChance = TagManager.AddNativeGameplayTag(FName("Attributes.Secondary.BlockChance"), FString("보조 능력치: 막기 확률"));
	GameplayTags.Attributes_Secondary_CritHitChance = TagManager.AddNativeGameplayTag(FName("Attributes.Secondary.CritHitChance"), FString("보조 능력치: 치명타 확률"));
	GameplayTags.Attributes_Secondary_CritHitDamage = TagManager.AddNativeGameplayTag(FName("Attributes.Secondary.CritHitDamage"), FString("보조 능력치: 치명타 피해"));
	GameplayTags.Attributes_Secondary_CritHitResistance = TagManager.AddNativeGameplayTag(FName("Attributes.Secondary.CritHitResistance"), FString("보조 능력치: 치명타 저항"));
	GameplayTags.Attributes_Secondary_HealthRegenerate = TagManager.AddNativeGameplayTag(FName("Attributes.Secondary.HealthRegenerate"), FString("보조 능력치: 체력 재생"));
	GameplayTags.Attributes_Secondary_ManaRegenerate = TagManager.AddNativeGameplayTag(FName("Attributes.Secondary.ManaRegenerate"), FString("보조 능력치: 마나 재생"));
	GameplayTags.Attributes_Secondary_MaxHealth = TagManager.AddNativeGameplayTag(FName("Attributes.Secondary.MaxHealth"), FString("보조 능력치: 최대 체력"));
	GameplayTags.Attributes_Secondary_MaxMana = TagManager.AddNativeGameplayTag(FName("Attributes.Secondary.MaxMana"), FString("보조 능력치: 최대 마나"));

	// Input Tags
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.LMB"),
		FString("Input Tag for Left Mouse Button")
	);
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.RMB"),
		FString("Input Tag for Right Mouse Button")
	);
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.One"),
		FString("Input Tag for Key 1")
	);
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Two"),
		FString("Input Tag for Key 2")
	);
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Three"),
		FString("Input Tag for Key 3")
	);
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Four"),
		FString("Input Tag for Key 4")
	);
	
	GameplayTags.Effect_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Effect.HitReact"),
		FString("Effect.HitReact")
	);


	// Damage Types
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage"),
	FString("Damage")
	);
	
	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage.Fire"),
	FString("Fire Damage Type")
	);
	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage.Lighting"),
	FString("Lighting Damage Type")
	);
	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage.Arcane"),
	FString("Arcane Damage Type")
	);
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage.Physical"),
	FString("Physical Damage Type")
	);


	// Damage Resistance
	GameplayTags.Attributes_Resistance_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Resistance.Fire"),
	FString("Fire Damage Resistance")
	);
	GameplayTags.Attributes_Resistance_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Resistance.Lighting"),
	FString("Lighting Damage Resistance")
	);
	GameplayTags.Attributes_Resistance_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Resistance.Arcane"),
	FString("Arcane Damage Resistance")
	);
	GameplayTags.Attributes_Resistance_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Resistance.Physical"),
	FString("Physical Damage Resistance")
	);


	// MAP DamageType To DamageResistance
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Fire, GameplayTags.Attributes_Resistance_Fire);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Lightning, GameplayTags.Attributes_Resistance_Lightning);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Arcane, GameplayTags.Attributes_Resistance_Arcane);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Physical, GameplayTags.Attributes_Resistance_Physical);
	

	// Abilities
	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Attack"),
		FString("Attack Ability")
	);
}



