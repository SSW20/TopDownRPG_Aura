// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "PassiveNiagaraComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UPassiveNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PassiveTag;

	UPassiveNiagaraComponent();

	void HandlePassive(const FGameplayTag& Tag, bool bActive);

protected:
	virtual void BeginPlay() override;
	void ActivateIfEquipped(UAuraAbilitySystemComponent* AuraASC);
};
