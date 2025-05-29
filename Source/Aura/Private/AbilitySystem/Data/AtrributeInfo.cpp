// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AtrributeInfo.h"

FAuraAttributeInfo UAtrributeInfo::FindAttributeInfoForTag(const FGameplayTag& Tag, bool blogNotFound) const
{
	for (const FAuraAttributeInfo& Info : AuraAttributeInfo)
	{
		if (Info.AttributeTag.MatchesTagExact(Tag))
		{
			return Info;
		}
	}

	if (blogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Tag Not Found for AttributeTag %s on Info %s"), *Tag.ToString(), *GetNameSafe(this));
	}
	return FAuraAttributeInfo();
}
