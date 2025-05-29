// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AtrributeInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraAttributeInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	FGameplayTag AttributeTag = FGameplayTag();
	
	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	FText AttributeName = FText();

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	FText AttributeDescription = FText();

	UPROPERTY(BlueprintReadOnly)
	float AttributeValue = 0.f;
};

/**
 * 
 */
UCLASS()
class AURA_API UAtrributeInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	FAuraAttributeInfo FindAttributeInfoForTag(const FGameplayTag& Tag, bool blogNotFound = false) const;
	
	UPROPERTY(EditAnyWhere,BlueprintReadOnly)
	TArray<FAuraAttributeInfo> AuraAttributeInfo;
};
