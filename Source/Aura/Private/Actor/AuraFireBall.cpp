// Copyright Druid Mechanics


#include "Actor/AuraFireBall.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AAuraFireBall::AAuraFireBall()
{
	ProjectileMovement->PrimaryComponentTick.bCanEverTick = false;
	ProjectileMovement->SetAutoActivate(false);
}

// Called when the game starts or when spawned
void AAuraFireBall::BeginPlay()
{
	if (Sphere == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AAuraFireBall::BeginPlay - Sphere component is nullptr!"));
		return; // Or handle the error appropriately
	}
	Super::BeginPlay();
	StartOutgoingTimeline();
}

void AAuraFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor)) return;

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulseVector = DeathImpulse;
			
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}


