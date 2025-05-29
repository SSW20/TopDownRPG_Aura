// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
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
};
