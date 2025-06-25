// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacter.h"
#include "Interaction/EnemyInterface.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "GameplayEffect.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */

UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();
public:

	// Start Enemy Interface
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	// End Enemy Interface

	// Start Combat Interface
	virtual int32 GetPlayerLevel() const override;
	// End Combat Interface
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;

	void HitReactTagChanged(const FGameplayTag Tag, int32 count);

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bIsReacting = false;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float MoveSpeed = 250.f;
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Character Class Defaults")
	float LifeSpan = 5.f;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
	TObjectPtr<UWidgetComponent> HealthBarWidgetComponent;



	virtual void Die() override;
private:
	
};
