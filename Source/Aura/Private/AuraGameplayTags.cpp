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

	
}



