// Copyright Druid Mechanics


#include "AbilitySystem/Abilities/AuraPassiveSpell.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UAuraPassiveSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
	{
		AuraASC->DeactivatePassiveTagDelegate.AddUObject(this, &UAuraPassiveSpell::HandleDeActivate);
	}
}

void UAuraPassiveSpell::HandleDeActivate(const FGameplayTag& Tag)
{
	if (AbilityTags.HasTagExact(Tag))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,true,true);
	}
}
