// Copyright Druid Mechanics


#include "Actor/AuraProjectile.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Aura/Aura.h"
// Sets default values
AAuraProjectile::AAuraProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true; // 투사체 복제 활성화

	Sphere = CreateDefaultSubobject<USphereComponent>("Projectile Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly); 
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Sphere->SetCollisionObjectType(ECC_Projectile);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Movement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

// Called when the game starts or when spawned
void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	//OnComponentBeginOverlap << Signature
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
	LoopSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopSound, GetRootComponent());
	SetLifeSpan(LifeSpan);
}


/*
	서버에서 Destroy를 호출하면 클라에게 Rep되어 보내져 호출됨 
	클라에서는 OnSphereOverlap이 먼저냐 Rep된 Destroy가 먼저 호출되냐 
	두 상황으로 갈리게 되는데 결과는 하나의 소리와 이펙트만 나오게 만들도록 했음
*/

void AAuraProjectile::Destroyed()
{
	if (!bIsPlaying && !HasAuthority())
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitEffect, GetActorLocation());
	}
	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitEffect, GetActorLocation());
	LoopSoundComponent->Stop();
	if (HasAuthority())
	{
		Destroy();

		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
		if (TargetASC)
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
	}
	else 
	{
		bIsPlaying = true;
	}
}


