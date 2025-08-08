// Copyright Druid Mechanics


#include "Game/LoadScreenSaveGame.h"


// 주어진 맵 이름과 일치하는 FSavedMap 구조체를 반환하는 함수
FSavedMap ULoadScreenSaveGame::GetSavedMapWithMapName(const FString& InMapName)
{
	for (const FSavedMap& Map : SavedMaps)
	{
		if (Map.MapAssetName == InMapName)
		{
			return Map;
		}
	}
	return FSavedMap();
}

// 주어진 맵 이름의 맵이 저장된 데이터에 있는지 확인하는 함수
bool ULoadScreenSaveGame::HasMap(const FString& InMapName)
{
	for (const FSavedMap& Map : SavedMaps)
	{
		if (Map.MapAssetName == InMapName)
		{
			return true;
		}
	}
	return false;
}