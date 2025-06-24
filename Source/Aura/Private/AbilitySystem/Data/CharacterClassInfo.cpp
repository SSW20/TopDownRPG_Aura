// Copyright Druid Mechanics


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetCharacterDefaultInfo(ECharacterClass CharacterClass)
{
	return CharacterDefaultInfoMap.FindChecked(CharacterClass);
}
