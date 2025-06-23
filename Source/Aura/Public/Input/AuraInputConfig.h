// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AuraInputConfig.generated.h"

/**
 * 
 */
class UInputAction;

USTRUCT(BlueprintType)
struct FAuraInputTag
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();
};

UCLASS()
class AURA_API UAuraInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	const UInputAction* FindInputActionByTag(const FGameplayTag& Tag) const;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TArray<FAuraInputTag> AbilityInputActions;
};
