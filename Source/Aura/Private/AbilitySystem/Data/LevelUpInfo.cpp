// Copyright Druid Mechanics


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::GetLevelByExpAmount(int32 ExpAmount)
{
	bool bSearching = true;
	int32 Level = 1;
	while (bSearching && LevelUpInfos.Num() > Level)
	{
		if (LevelUpInfos[Level].LevelUpExpAmount <= ExpAmount) Level++;
		else bSearching = false;
	}
	return Level;
}

int32 ULevelUpInfo::GetExpByLevel(int32 Level)
{
	if (Level >= LevelUpInfos.Num())
	{
		return 0;
	}
	else
	{
		return LevelUpInfos[Level].LevelUpExpAmount;
	}
}

