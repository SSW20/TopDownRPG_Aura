// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"
#include "NiagaraSystem.h"
#include "Components/AudioComponent.h"
#include "GameplayEffectTypes.h"
#include "AuraProjectile.generated.h"


UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAuraProjectile();
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USphereComponent> Sphere;
	/*
	*	OverlappedComponent: 이벤트를 발생시킨 컴포넌트(현재 객체의 컴포넌트).
		OtherActor: 충돌한 다른 액터.
		OtherComp: OtherActor에서의 충돌한 컴포넌트.
		OtherBodyIndex: 충돌한 다른 액터의 본체 인덱스 (본체가 여러 개인 경우).
		bFromSweep: 스위프트 충돌 여부 (충돌을 스위프트로 검출했는지 여부).
		SweepResult: 충돌 정보를 제공하는 FHitResult 구조체.
	*/
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	
	UPROPERTY(EditAnywhere, Category = "Hit")
	TObjectPtr<UNiagaraSystem> HitEffect;
	UPROPERTY(EditAnywhere, Category = "Hit")
	TObjectPtr<USoundBase> HitSound;
	UPROPERTY(EditAnywhere, Category = "Loop")
	TObjectPtr<USoundBase> LoopSound;
	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopSoundComponent;
	bool bIsPlaying = false;
	
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 10.f;


};
