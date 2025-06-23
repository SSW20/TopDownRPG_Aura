// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AuraAttributeSet.generated.h"

// ATTRIBUTE_ACCESSORS 매크로
// 언리얼 엔진의 어빌리티 시스템에서 Attribute에 접근하고 조작하는 데 사용되는 getter, setter, 초기화 함수들을 자동으로 생성하는 코드를 간결하게 만들어주는 매크로
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

//DECLARE_DELEGATE_RetVal() << TBaseStaticDelegateInstance로 대체

USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties() {}

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;

	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY()
	AController* TargetController = nullptr;

	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};


UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UAuraAttributeSet();

	// 멀티플레이어 게임에서 어떤 속성들을 네트워크를 통해 복제 (Replication) 할지 결정하는 역할
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	//TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;


	//FGameplayAttribute타입을 변환하고 함수 포인터를 저장할 수 있는 델레게이트, 파라미터는 받지 않음 () 식으로 되어있기 때문에
	//FFuncPtr  있다 --> 순수한 함수 포인터 타입 / 없다 --> 그냥 델레게이트임
	//using TStaticFuncPtr << 이거로 TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr를 대체함
	//T는 직접 타입 지정
	template<class T>
	using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;
	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;
	
	//TBaseStaticDelegateInstance<FGameplayAttribute(), FDefaultDelegateUserPolicy>::FFuncPtr StaticFunc;
	//TMap<FGameplayTag, TBaseStaticDelegateInstance<FGameplayAttribute(), FDefaultDelegateUserPolicy>::FFuncPtr> TagToAttributes;


	/*
	*			Vital Attribute
	*/

	//RepNotify 사용  --> 변수가 복제될 때 자동으로 호출됨 EX) Health가 변경되면 OnRep_Health가 호출됨
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana);

	/*
	*			Primary Attribute	(힘, 지능, 관통력, 활기)
	*/
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Primary Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Strength);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Primary Attributes")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Intelligence);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resilience, Category = "Primary Attributes")

	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Resilience);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Vigor, Category = "Primary Attributes")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Vigor);


	/*
				Secondary Attribute
	*/

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Secondary Attributes")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Armor);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetration, Category = "Secondary Attributes")
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ArmorPenetration);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BlockChance, Category = "Secondary Attributes")
	FGameplayAttributeData BlockChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, BlockChance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritHitChance, Category = "Secondary Attributes")
	FGameplayAttributeData CritHitChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CritHitChance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritHitDamage, Category = "Secondary Attributes")
	FGameplayAttributeData CritHitDamage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CritHitDamage);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritHitResist, Category = "Secondary Attributes")
	FGameplayAttributeData CritHitResist;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CritHitResist);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegenerate, Category = "Secondary Attributes")
	FGameplayAttributeData HealthRegenerate;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, HealthRegenerate);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRegenerate, Category = "Secondary Attributes")
	FGameplayAttributeData ManaRegenerate;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ManaRegenerate);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Secondary Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Secondary Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana);




	// 인자를 0 ~ 1개 받을 수 있음 -> 업데이트 이전 값이 들어감

	/*
				Vital Attribute
	*/ 
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;


	/*
				Primary Attribute
	*/
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;
	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;
	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;
	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;



	/*
				Secondary Attribute
	*/
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;
	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const;
	UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const;
	UFUNCTION()
	void OnRep_CritHitChance(const FGameplayAttributeData& OldCritChance) const;
	UFUNCTION()
	void OnRep_CritHitDamage(const FGameplayAttributeData& OldCritDMG) const;
	UFUNCTION()
	void OnRep_CritHitResist(const FGameplayAttributeData& OldCritResist) const;
	UFUNCTION()
	void OnRep_HealthRegenerate(const FGameplayAttributeData& OldHealthRegerate) const;
	UFUNCTION()
	void OnRep_ManaRegenerate(const FGameplayAttributeData& OldManaRegerate) const;
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;


private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
};
