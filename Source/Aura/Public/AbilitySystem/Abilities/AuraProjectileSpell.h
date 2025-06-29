// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageGameplayAbility.h"
#include "Actor/AuraProjectile.h"
#include "AuraProjectileSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraProjectileSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

	/*
		GameplayAbilitySpecHandle (어빌리티 명세서 핸들), 
		GameplayAbilityActorInfo (소유 액터 및 컨트롤러 정보), 
		GameplayAbilityActivationInfo (활성화 정보), 
		GameplayEventData (이벤트로 활성화 시 데이터)
	*/
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData);
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void SpawnProjectileInBluePrint(const FVector& TargetLocation);


	
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<AAuraProjectile> ProjectileClass;
};
