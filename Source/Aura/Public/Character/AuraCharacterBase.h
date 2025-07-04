// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Materials/MaterialInstance.h"
#include "GameplayEffect.h"
#include "Interaction/CombatInterface.h"
#include "UI/Widget/FloatingDamageText.h"
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

	virtual void InitializeDefaultAttributes() const;
	void ApplyEffectToSelfInit(TSubclassOf<UGameplayEffect> EffectClass, float level) const;

	/* Combat Interface */
	virtual FVector GetCombatSocketLotation_Implementation();
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetActor_Implementation() override;
	virtual void Die() override;

	bool bDead = false;
	/* End Combat Interface */

	
	// NetMulticast : "네트워크를 통해 멀티캐스트 됨. 서버에서 호출되면 서버 자신과 연결된 모든 클라이언트 인스턴스에서 동시에 실행됨.
	// Reliable : 이 네트워크 RPC 호출은 네트워크 상에서 발생하더라도 해당 함수 호출은 반드시 목적지에 도착하여 실행됨을 보장함.
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();
};
