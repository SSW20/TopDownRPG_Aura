// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Materials/MaterialInstance.h"
#include "GameplayEffect.h"
#include "Interaction/CombatInterface.h"
#include "Abilities/GameplayAbility.h"
#include "AuraCharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface,  public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAuraCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo();
	virtual void AddGameplayAbilities() const; 
	virtual FVector GetCombatSocketLotation();
	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponSocketName;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;

	UPROPERTY(EditDefaultsOnly, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartGameplayAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Anime")
	TObjectPtr<UAnimMontage> HitReaction;

	UPROPERTY(EditDefaultsOnly, Category = "Disolve")
	TObjectPtr<UMaterialInstance> CharacterMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Disolve")
	TObjectPtr<UMaterialInstance> WeaponMaterial;
	
	void Disolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartCharacterDisolveTimeLine(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDisolveTimeLine(UMaterialInstanceDynamic* DynamicMaterialInstance);
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual void InitializeDefaultAttributes() const;
	void ApplyEffectToSelfInit(TSubclassOf<UGameplayEffect> EffectClass, float level) const;

	virtual void Die() override;

	// NetMulticast : ��Ʈ��ũ�� ���� ��Ƽĳ��Ʈ�� ��, �������� ȣ��Ǹ�, ���� �ڽŰ� ���� ����� ��� Ŭ���̾�Ʈ �ӽſ��� ���ÿ� ����
	// Reliable : �� ��Ʈ��ũ RPC ȣ�⿡ ������ �߻��ϴ��� ������ �� �Լ� ȣ���� �ݵ�� �������� �����Ͽ� ����ǵ��� ����
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();
};
