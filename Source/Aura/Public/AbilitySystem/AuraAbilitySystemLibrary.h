// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AuraAbilitySystemLibrary.generated.h"

/**
 * 
 */
class UOverlayWidgetController;
class UAttributeMenuWidgetController;
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "AbilitySystem|OverlayWidget")
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContext);

	UFUNCTION(BlueprintPure, Category = "AbilitySystem|AttributeMenu")
	static UAttributeMenuWidgetController* GetAttributeMenuController(const UObject* WorldContext); 

	UFUNCTION(BlueprintCallable , Category = "AbilitySystem|Character Default Attributes")
	static void InitDefaultAttributes(const UObject* WorldContext, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "AbilitySystem|Character Default Abilities")
	static void InitDefaultAbilities(const UObject* WorldContext, UAbilitySystemComponent* ASC);
	
	
	// FGameplayEffectContextHandle 만 있으면 접근해서 FAuraGameplayEffectContext 의 함수 호출할 수 있음
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

		// 기본적으로 언리얼엔진에서는 인자로 CONST가 아니면 OUT파라미터로 인식 --> 이를 막기 위해 UPARAM(ref) 사용
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);
};
