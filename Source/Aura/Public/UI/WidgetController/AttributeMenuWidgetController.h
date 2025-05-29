// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AbilitySystem/Data/AtrributeInfo.h"
#include "GameplayTagContainer.h" 
#include "AbilitySystem/AuraAttributeSet.h" 
#include "AttributeMenuWidgetController.generated.h"
/**
 * 
 */
struct FAuraAttributeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAuraAttributeInfoSignature, const FAuraAttributeInfo&, Info);

UCLASS()
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category = "GAS|Tags")
	FAuraAttributeInfoSignature AttributeInfoDelegate;

	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAtrributeInfo> AttributeInfo;

private:
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;
};
