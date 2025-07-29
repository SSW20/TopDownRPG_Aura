// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AuraProjectile.h"
#include "GameFramework/Actor.h"
#include "AuraFireBall.generated.h"

UCLASS()
class AURA_API AAuraFireBall : public AAuraProjectile
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAuraFireBall();

	UFUNCTION(BlueprintImplementableEvent)
	void StartOutgoingTimeline();
	UPROPERTY(BlueprintReadWrite)
	FDamageEffectParams ExplosionDamageParams;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

};
