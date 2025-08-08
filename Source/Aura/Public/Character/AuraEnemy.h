// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacter.h"
#include "Interaction/EnemyInterface.h"
#include "Components/WidgetComponent.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "Interaction/HighlightInterface.h"
#include "AuraEnemy.generated.h"

class AAuraAIController;
/**
 * 
 */
class UBehaviorTree;
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface, public IHighlightInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();
public:
	/** Highlight Interface */
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) override;
	/** End Highlight Interface */
	
	// Start Enemy Interface
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	// End Enemy Interface

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;
	
	// Start Combat Interface
	virtual int32 GetPlayerLevel_Implementation() const override;
	virtual void SetIsShocking_Implementation(bool IsShocking) override;
	// End Combat Interface

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnLoot();
	
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;

	void HitReactTagChanged(const FGameplayTag Tag, int32 count);

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bIsReacting = false;

	void SetLevel(const int32 InLevel) {Level = InLevel;}
	void SetCharacterClass(const ECharacterClass InClass) { CharacterClass = InClass; }
	
	virtual void PossessedBy(AController* NewController) override;
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;
	virtual void StunTagChanged(const FGameplayTag Tag, int32 Count) override;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Character Class Defaults")
	float LifeSpan = 5.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
	TObjectPtr<UWidgetComponent> HealthBarWidgetComponent;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAuraAIController> AIController;

	virtual void Die(const FVector& DeathImpulseVector) override;
private:
	
};
