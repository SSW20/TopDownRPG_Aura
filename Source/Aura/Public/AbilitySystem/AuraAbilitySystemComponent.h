// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "AuraAbilitySystemComponent.generated.h"
/**
 * 
 */
// FForEachAbility: const FGameplayAbilitySpec& 타입의 매개변수 하나를 받는 싱글캐스트 델리게이트
// 이 델리게이트는 어빌리티 목록을 순회하며 각 어빌리티 스펙에 대해 특정 작업을 수행할 때 사용

//FAbilitiesGiven: UAuraAbilitySystemComponent* 타입의 매개변수 하나를 받는 멀티캐스트 델리게이트
// 이 델리게이트는 주로 어빌리티가 부여되었음을 알리는 이벤트에 사용

class UAuraAbilitySystemComponent;
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer&);
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven)
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&)
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAblityStatusTags, const FGameplayTag&  /*Ability Tag*/, const FGameplayTag& /*Status Tag*/, int32 /*Level*/);
DECLARE_MULTICAST_DELEGATE_FourParams(FAblityInfoTags, const FGameplayTag&  /*Ability Tag*/,
	const FGameplayTag& /*Status Tag*/, const FGameplayTag&  /*Input Slot Tag*/, const FGameplayTag& /*Prev Input Slot Tag*/);

UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	FEffectAssetTags EffectAssetTags;
	FAbilitiesGiven AbilitiesGivenDelegate;
	FAblityStatusTags AbilityStatusTagsDelegate;
	FAblityInfoTags AbilityInfoTagsDelegate;
	bool bIsStartUpAbilitiesBroadCasted = false;
	
	void AbilityActorInfoSet();
	void AddGameplayAbilities(const TArray<TSubclassOf<UGameplayAbility>>& GameplayAbilities);
	void AddPassiveGameplayAbilities(const TArray<TSubclassOf<UGameplayAbility>>& PassiveGameplayAbilities);
	void ForEachAbility(const FForEachAbility& ForEachDelegate);
	void UpdateAbilityStatus(int32 Level);
	
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& Spec);
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& Spec);
	static FGameplayTag GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	FGameplayTag GetStatusTagFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetInputFromAbilityTag(const FGameplayTag& AbilityTag);

	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& Tag);
	FGameplayAbilitySpec* GetSpecFromInputTag(const FGameplayTag& InputTag);

	void ClearAbilitiesFromInputTag(const FGameplayTag& InputTag);
	void ClearInputTagBySpec(FGameplayAbilitySpec* AbilitySpec);
	bool HasInputTag(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& InputTag);
	
	void PlayIfHeld(const FGameplayTag& InputTag);
	void PlayIfReleased(const FGameplayTag& InputTag);

	void UpgradeAttributes(const FGameplayTag& Tag);

	UFUNCTION(Server,Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& Tag);

	UFUNCTION(Server,Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);

	UFUNCTION(Server,Reliable)
	void ServerEquipAbility(const FGameplayTag& SlotInputTag, const FGameplayTag& AbilityTag);

	bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription);
	void ClientEquipAbility(const FGameplayTag&  AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag&  InputSlotTag, const FGameplayTag& PrevInputSlotTag);
protected:
	void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle EffectHandle);

	virtual void OnRep_ActivateAbilities() override;
	
	UFUNCTION(Client, Reliable)
	void ClientUpdateStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 Level);

	
};


