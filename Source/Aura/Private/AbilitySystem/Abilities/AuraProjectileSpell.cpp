// Copyright Druid Mechanics


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "AuraGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	

	/*
		GameplayAbilitySpecHandle (어빌리티 명세서 핸들),
		GameplayAbilityActorInfo (소유 액터 및 컨트롤러 정보),
		GameplayAbilityActivationInfo (활성화 정보),
		GameplayEventData (이벤트로 활성화 시 데이터)
	*/
}

void UAuraProjectileSpell::SpawnProjectileInBluePrint(const FVector& TargetLocation)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(CombatInterface->GetCombatSocketLotation());

		//TODO : Rotation값 설정
		FRotator Rotation = (TargetLocation - CombatInterface->GetCombatSocketLotation()).Rotation();

		SpawnTransform.SetRotation(Rotation.Quaternion());


		//SpawnActorDeferred: 액터를 즉시 스폰하지 않고, 모든 속성을 설정한 후 나중에 FinishSpawning으로 스폰을 완료할 수 있게 해주는 함수
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);

		//TODO : 맞았을 때 게임플레이 이펙트 
		GetAvatarActorFromActorInfo();
		UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffectClass, 1.f, ContextHandle);

		//	FGameplayEffectContext의 정보들을 최대한 채우는 과정 (Ability / SourceObject / Actors / HitResult ...)
		ContextHandle.SetAbility(this);
		ContextHandle.AddSourceObject(Projectile);
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		ContextHandle.AddActors(Actors);
		FHitResult HitResult;
		HitResult.Location = TargetLocation;
		ContextHandle.AddHitResult(HitResult);
		

		// Gameplay Tag : Key / Magnitude : Value
		FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		for (auto& DamageType : DamageTypes)
		{
			SpecHandle = UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType.Key,DamageType.Value.GetValueAtLevel(GetAbilityLevel()));
		}
		
		Projectile->DamageEffectSpecHandle = SpecHandle;
		Projectile->FinishSpawning(SpawnTransform);
	}
}
