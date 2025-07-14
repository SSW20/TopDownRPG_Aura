// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilityInfo.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FAuraAbilityInfo
{
	GENERATED_BODY()
public:
	// 어빌리티를 고유하게 식별하는 게임플레이 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityTag = FGameplayTag();

	//어빌리티에 할당된 입력 태그 (예: 좌클릭, 우클릭)
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();

	// 쿨다운 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag CooldownTag = FGameplayTag();

	
	//const class는 특정 타입이 클래스임을 명시하는 동시에, 해당 타입의 const를 나타내는 전방 선언의 한 형태
	
	//UTexture2D는 2D 이미지 데이터를 나타내는 언리얼 엔진의 에셋 타입
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UTexture2D> SkillIcon = nullptr;

	//UMaterial의 인터페이스 UMaterial이거나 상속받는 객체를 사용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const class UMaterialInterface> BackgroundMaterial = nullptr;
	
	
};

UCLASS()
class AURA_API UAbilityInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	//여러 어빌리티 정보를 저장
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AbilityInfo")
	TArray<FAuraAbilityInfo> AbilityInfos;

	//특정 게임플레이 태그(AbilityTag 또는 InputTag)를 사용하여 FAuraAbilityInfo를 찾는 함수
	FAuraAbilityInfo FindAbilityInfoByTag(const FGameplayTag& Tag, bool bLogNotFound = false) const;
	
};
