// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacter.h"
#include "Interaction/EnemyInterface.h"
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

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	
};
