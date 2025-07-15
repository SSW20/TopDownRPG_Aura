// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
* //							UPlayerInterface를 생성하는 이유
//		UAuraAttributeSet이 플레이어의 경험치(XP)를 직접 PlayerState에 추가하는 것을 피하기 위함.
//		1. 순환 종속성 방지: PlayerState가 AttributeSet에 의존하고, AttributeSet이 다시 PlayerState에 의존하는 순환 참조를 막습니다.
//		2. 느슨한 결합: AttributeSet은 PlayerState의 구체적인 구현(XP 변수 위치 등)을 몰라도,
//			UPlayerInterface을 통해 XP 추가 기능을 요청할 수 있게 됩니다.
//		3. 책임 분리: AttributeSet은 전투 속성에 집중하고, PlayerState는 플레이어 진행 상태 관리에 집중하도록 합니다.
//
		즉, AttributeSet은 "XP가 들어왔다"는 이벤트를 알리고, UPlayerInterface를 구현한 캐릭터가
//		실제로 PlayerState의 XP를 업데이트하는 로직을 담당하게 됩니다.

 */
class AURA_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void AddExp(int32 Exp);
};
