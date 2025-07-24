// Copyright Druid Mechanics


#include "AbilitySystem/Abilities/AuraMeleeAttack.h"

FDamageEffectParams UAuraMeleeAttack::DetermineKnockBack(AActor* TargetActor, FDamageEffectParams& Params)
{
	if (TargetActor)
	{
		bool bKnockback = FMath::RandRange(1, 100) <= Params.KnockbackChance;
		if (bKnockback)
		{
			FRotator KnockbackRotator = GetAvatarActorFromActorInfo()->GetActorRotation();
			KnockbackRotator.Pitch = 45.f;
			const FVector KnockbackForce = KnockbackRotator.Vector() * Params.KnockbackMagnitude;
			Params.KnockbackImpulseVector = KnockbackForce;
		}
	}
	return Params;
}
