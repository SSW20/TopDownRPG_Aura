// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Summon")
	int32 SummonCount = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Summon")
	TArray<TSubclassOf<APawn>> SummonPawns;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Summon")
	float MinSummonDistance = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Summon")
	float MaxSummonDistance = 400.f;

	UPROPERTY(EditDefaultsOnly, Category = "Summon")
	float SummonSpread = 90.f;

	UFUNCTION(BlueprintPure)
	TSubclassOf<APawn> GetRandomSummonPawn() const;
	
};
