// Copyright Druid Mechanics


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

class UAbilitySystemComponent;
class ICombatInterface;

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}


void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
	UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()));
	if (AuraASC)
	{
		AuraASC->ActivatePassiveDelegate.AddUObject(this, &UPassiveNiagaraComponent::HandlePassive);
	}
	else
	{
		if (CombatInterface)
		{
			CombatInterface->GetASCRegistered().AddLambda([this](UAbilitySystemComponent* ASC)
			{
				if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(ASC))
				{
					AuraASC->ActivatePassiveDelegate.AddUObject(this, &UPassiveNiagaraComponent::HandlePassive);
				}
			});
		}
	}
	
}
void UPassiveNiagaraComponent::HandlePassive(const FGameplayTag& Tag, bool bActive)
{
	if (PassiveTag.MatchesTagExact(Tag))
	{
		if (bActive)
		{
			Activate();
		}
		else
		{
			Deactivate();
		}
	}
}
