// Copyright Druid Mechanics


#include "Actor/AuraProjectile.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
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

void AAuraProjectile::OnHit()
{
	UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitEffect, GetActorLocation());
	if (LoopSoundComponent)
	{
		LoopSoundComponent->Stop();	
		LoopSoundComponent->DestroyComponent();
	}
}

void AAuraProjectile::Destroyed()
{
	//코드가 서버가 아닌 클라이언트에서 실행되고 있을 때.
    //서버는 OnSphereOverlap에서 이미 효과를 처리하거나, 클라이언트의 중복 재생을 막음
	//서버의 Destroy가 먼저 도착했을 때 클라이언트가 효과를 재생
	if (LoopSoundComponent)
	{
		LoopSoundComponent->Stop();	
		LoopSoundComponent->DestroyComponent();
	}
	
	if (!bIsPlaying && !HasAuthority()) OnHit();
	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//투사체가 발사자(주인) 자신을 때리는 경우를 방지, 
	//만약 코드가 없었다면 자신을 맞추고 Destroy --> 다른 액터를 맞춰도 이미 사라진 상태임
	AActor* SourceActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	if (SourceActor == OtherActor) return;
	if (UAuraAbilitySystemLibrary::IsFriend(SourceActor,OtherActor)) return;
	
	//이 충돌에 대한 효과가 아직 재생되지 않았다면
    //클라이언트가 충돌을 예측하여 효과를 먼저 재생하는 시나리오
	if (!bIsPlaying) OnHit();

	//서버 권한 하에 데미지 적용 및 투사체 파괴
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			DamageEffectParams.DeathImpulseVector = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;

			bool bKnockback = FMath::RandRange(1, 100) <= DamageEffectParams.KnockbackChance;
			if (bKnockback)
			{
				FRotator KnockbackRotator = GetActorRotation();
				KnockbackRotator.Pitch = 45.f;
				const FVector KnockbackForce = KnockbackRotator.Vector() * DamageEffectParams.KnockbackMagnitude;
				DamageEffectParams.KnockbackImpulseVector = KnockbackForce;
			}
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}

		Destroy();
	}

	//클라이언트에서 충돌 처리 후 플래그 설정
	else bIsPlaying = true;

}


