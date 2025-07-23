// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "DebuffNiagaraComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UDebuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere)
	FGameplayTag DebuffTag;

	UDebuffNiagaraComponent();

	void DebuffTagChanged(const FGameplayTag Tag, int32 count);

protected:
	virtual void BeginPlay() override;
	
};
