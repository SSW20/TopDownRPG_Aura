// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Data/LootTiers.h"
#include "Game/LoadScreenSaveGame.h"
#include "UI/HUD/AuraHUD.h"
#include "AuraAbilitySystemLibrary.generated.h"

class USpellMenuWidgetController;
/**
 * 
 */
class UOverlayWidgetController;
class UAttributeMenuWidgetController;
class UAbilityInfo;



UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "AbilitySystem|OverlayWidget", meta = ( DefaultToSelf = "WorldContextObject"))
	static bool GetWidgetControllerParams(const UObject* WorldContext, FWidgetControllerParams& OutWidgetControllerParams, AAuraHUD*& OutHUD);
	
	UFUNCTION(BlueprintPure, Category = "AbilitySystem|OverlayWidget", meta = ( DefaultToSelf = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContext);

	UFUNCTION(BlueprintPure, Category = "AbilitySystem|AttributeMenu", meta = ( DefaultToSelf = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuController(const UObject* WorldContext);
	
	UFUNCTION(BlueprintPure, Category = "AbilitySystem|SpellMenu", meta = ( DefaultToSelf = "WorldContextObject"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContext);
	
	UFUNCTION(BlueprintCallable , Category = "AbilitySystem|Character Default Attributes")
	static void InitDefaultAttributes(const UObject* WorldContext, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);
	static void InitializeDefaultAttributesFromSaveData(const UObject* WorldContextObject, UAbilitySystemComponent* ASC,
	                                             ULoadScreenSaveGame* SaveGame);

	UFUNCTION(BlueprintCallable, Category = "AbilitySystem|Character Default Abilities")
	static void InitDefaultAbilities(const UObject* WorldContext, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass);
	
	
	// FGameplayEffectContextHandle 만 있으면 접근해서 FAuraGameplayEffectContext 의 함수 호출할 수 있음
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsDebuffSuccess(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static float GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static float GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static float GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static FGameplayTag GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static FVector GetDeathImpulsVector(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static FVector GetKnockbackImpulsVector(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static float GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static float GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static FVector GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle);
	
	// 기본적으로 언리얼엔진에서는 인자로 CONST가 아니면 OUT파라미터로 인식 --> 이를 막기 위해 UPARAM(ref) 사용
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsDebuffSuccess(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsDebuffSuccess);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetDebuffDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InDamage);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetDebuffDuration(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InDuration);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetDebuffFrequency(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InFrequency);
	
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetDamageType(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType);
	
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetDeathImpulseVector(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InVector);
	
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetKnockImpulseVector(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InVector);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsRadialDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsRadialDamage);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageInnerRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InInnerRadius);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageOuterRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InOuterRadius);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageOrigin(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InOrigin);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static void GetLivePlayersWithInRadius(const UObject* WorldContextObject, TArray<AActor*> &OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static void GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors, TArray<AActor*>& OutClosestTargets, const FVector& Origin);
	
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static bool IsFriend(AActor* FirstActor, AActor* SecondActor);

	static int32 GetExpRewardByClassAndLevel(const UObject* WorldContext, ECharacterClass CharacterClass, float Level);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& Params);

	// Spawn Proejectiles
	UFUNCTION(BlueprintPure,  Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static TArray<FRotator> GetEvenlySpreadRotators(const FVector& Forward, const FVector& Axis, float Spread, float ProjectileCount);
	
	UFUNCTION(BlueprintPure,  Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static TArray<FVector> GetEvenlySpreadVectors(const FVector& Forward, const FVector& Axis, float Spread, float ProjectileCount);

	// Loot Tier
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|CharacterClassDefaults", meta = (DefaultToSelf = "WorldContextObject"))
	static ULootTiers* GetLootTiers(const UObject* WorldContextObject);
};
