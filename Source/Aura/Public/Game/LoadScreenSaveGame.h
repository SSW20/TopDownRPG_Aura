// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

/**
 * 
 */

// 스위치 위젯을 위한 인덱스
UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	EnterName,
	Taken
};


// 어빌리티의 정보를 담을 저장변수 << Ability / Tag / Status / Slot / Type / Level 을 저장함 
USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ClassDefaults")
	TSubclassOf<UGameplayAbility> GameplayAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityStatus = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilitySlot = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityType = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 AbilityLevel = 1;
};

// 액터의 정보를 저장하고 직렬화 함 << 이름 / 위치,회전 을 저장
USTRUCT()
struct FSavedActor
{
	GENERATED_BODY()

	UPROPERTY()
	FName ActorName = FName();

	UPROPERTY()
	FTransform Transform = FTransform();

	// Serialized 된 액터의 변수들을 저장하는 바이트 배열 - 'SaveGame' 지정자가 있는 변수들만 이 배열에 포함
	UPROPERTY()
	TArray<uint8> Bytes;
};

inline bool operator==(const FSavedActor& Left, const FSavedActor& Right)
{
	return Left.ActorName == Right.ActorName;
}

// 맵의 저장 상태를 담는 구조체
USTRUCT()
struct FSavedMap
{
	GENERATED_BODY()

	// 맵의 에셋 이름을 저장하여 맵을 식별
	UPROPERTY()
	FString MapAssetName = FString();

	// 해당 맵에 저장된 모든 액터 데이터의 배열
	UPROPERTY()
	TArray<FSavedActor> SavedActors;
};


// Add Unique 때문에 해줘야됨
inline bool operator==(const FSavedAbility& Left, const FSavedAbility& Right)
{
	return Left.AbilityTag.MatchesTagExact(Right.AbilityTag);
}

UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	
	UPROPERTY()
	FString SlotName = FString();

	UPROPERTY()
	int32 SlotIndex = 0;

	UPROPERTY()
	FString PlayerName = FString();

	UPROPERTY()
	FString MapName = FString("Default Map Name");

	UPROPERTY()
	FString MapAssetName = FString("Default Map Asset Name");

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = Vacant;

	UPROPERTY()
	FName PlayerStartTag;

	UPROPERTY()
	bool bFirstTimeSaved = false;
	
	/* Player */
	
	UPROPERTY()
	int32 PlayerLevel = 1;

	UPROPERTY()
	int32 XP = 0;

	UPROPERTY()
	int32 SpellPoints = 0;

	UPROPERTY()
	int32 AttributePoints = 0;

	UPROPERTY()
	float Strength = 0;

	UPROPERTY()
	float Intelligence = 0;

	UPROPERTY()
	float Resilience = 0;

	UPROPERTY()
	float Vigor = 0;

	UPROPERTY()
	TArray<FSavedAbility> SavedAbilities;

	/* Saved Map */
	UPROPERTY()
	TArray<FSavedMap> SavedMaps;

	FSavedMap GetSavedMapWithMapName(const FString& InMapName);
	bool HasMap(const FString& InMapName);
};
