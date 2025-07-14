// Copyright Druid Mechanics


#include "AbilitySystem/Data/AbilityInfo.h"

#include "Aura/AuraLogChannels.h"

//AbilityInfos 배열을 순회하며 태그와 일치하는 어빌리티 정보를 찾고
//찾지 못했으면 오류 로그를 출력
FAuraAbilityInfo UAbilityInfo::FindAbilityInfoByTag(const FGameplayTag& Tag, bool bLogNotFound) const
{
	for (FAuraAbilityInfo AbilityInfo : AbilityInfos)
	{
		if (AbilityInfo.AbilityTag.MatchesTagExact(Tag) || AbilityInfo.InputTag.MatchesTagExact(Tag))
		{
			return AbilityInfo;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("Ability Tag not found, Tag : %s, AbilityInfo : %s" ), *Tag.ToString(), *GetNameSafe(this));
	}
	return FAuraAbilityInfo();
}

