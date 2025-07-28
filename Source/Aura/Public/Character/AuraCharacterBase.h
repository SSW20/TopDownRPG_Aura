// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Materials/MaterialInstance.h"
#include "GameplayEffect.h"
#include "NiagaraSystem.h"
#include "Interaction/CombatInterface.h"
#include "UI/Widget/FloatingDamageText.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "AuraCharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface,  public ICombatInterface
{
	GENERATED_BODY() 

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Sets default values for this character's properties
	AAuraCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	TArray<FTaggedMontage> TaggedMontages;

	/* Combat Interface */
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& Tag) override;
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetActor_Implementation() override;
	virtual void Die(const FVector& DeathImpulseVector) override;
	virtual FOnDeathSignature& GetOnDeathDelegate() override;
	virtual TArray<FTaggedMontage> GetTaggedMontages_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& Tag) override;
	virtual int32 GetSummonCount_Implementation() override;
	virtual void IncreaseSummonCount_Implementation(int32 AddValue) override;
	virtual ECharacterClass GetCharacterClass_Implementation() override;
	virtual FOnASCRegistered& GetASCRegistered() override;
	virtual USkeletalMeshComponent* GetWeapon_Implementation() override;
	virtual void SetIsShocking_Implementation(bool IsShocking) override;
	virtual bool IsShocking_Implementation() const override;
	virtual FOnDamageSignature& GetOnDamageSignature() override;
	/* End Combat Interface */

	UPROPERTY(ReplicatedUsing=OnRep_Stunned, BlueprintReadOnly)
	bool bIsStunned = false;

	UPROPERTY(ReplicatedUsing=OnRep_Burned, BlueprintReadOnly)
	bool bIsBurned = true;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsShocking = false;
	
	UFUNCTION()
	virtual void OnRep_Stunned();

	UFUNCTION()
	virtual void OnRep_Burned();
	
protected:
	virtual void StunTagChanged(const FGameplayTag Tag, int32 Count);
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Combat")
	float MoveSpeed = 600.f;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo();
	virtual void AddGameplayAbilities() const; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightHandSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftHandSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName TailSocketName;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnyWhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;

	UPROPERTY(EditDefaultsOnly, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartGameplayAbilities;
	
	UPROPERTY(EditDefaultsOnly, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartPassiveGameplayAbilities;
	
	UPROPERTY(EditDefaultsOnly, Category = "Anime")
	TObjectPtr<UAnimMontage> HitReaction;

	UPROPERTY(EditDefaultsOnly, Category = "Disolve")
	TObjectPtr<UMaterialInstance> CharacterMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Disolve")
	TObjectPtr<UMaterialInstance> WeaponMaterial;
	
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;
	
	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartCharacterDisolveTimeLine(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDisolveTimeLine(UMaterialInstanceDynamic* DynamicMaterialInstance);

	virtual void InitializeDefaultAttributes() const;
	void ApplyEffectToSelfInit(TSubclassOf<UGameplayEffect> EffectClass, float level) const;

	UPROPERTY(EditAnywhere, Category="Combat")
	UNiagaraSystem* BloodEffect;
	
	bool bDead = false;
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="Combat")
	USoundBase* DeathSoundBase;

	
	// NetMulticast : "네트워크를 통해 멀티캐스트 됨. 서버에서 호출되면 서버 자신과 연결된 모든 클라이언트 인스턴스에서 동시에 실행됨.
	// Reliable : 이 네트워크 RPC 호출은 네트워크 상에서 발생하더라도 해당 함수 호출은 반드시 목적지에 도착하여 실행됨을 보장함.
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath(const FVector& DeathImpuseVector);

	// Minions
	int32 SummonCount = 0;

	// ASC Delegate
	FOnASCRegistered ASCRegisteredDelegate;
	
	FOnDeathSignature OnDeathDelegate;

	FOnDamageSignature OnDamageDelegate;

	// Debuff Niagara Component
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDebuffNiagaraComponent> BurnNiagaraComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDebuffNiagaraComponent> StunNiagaraComponent;

	// Passive Niagara Component
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> HaloNiagaraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> LifeStealNiagaraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> ManaStealNiagaraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> EffectAttachComponent;

	// Take Damage Override
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};

inline void AAuraCharacterBase::OnRep_Stunned()
{
}


