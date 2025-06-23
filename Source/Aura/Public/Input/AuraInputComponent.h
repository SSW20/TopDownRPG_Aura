// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "AuraInputConfig.h"
#include "AuraInputComponent.generated.h"

/**
 * 커스텀 입력 컴포넌트 클래스
	// InputConfig를 받아 어빌리티 액션을 콜백 함수에 바인딩하는 템플릿 함수
    // UserClass: 콜백 함수를 소유하는 객체의 타입 (보통 플레이어 컨트롤러)
    // PressedFuncType, ReleasedFuncType, HeldFuncType: 각 콜백 함수의 타입


			// Held (지속) 액션 바인딩
			// Triggered 이벤트: 입력이 지속되는 동안 매 프레임 호출

			// Pressed (시작) 액션 바인딩
			// Started 이벤트: 입력이 처음 눌렸을 때 한 번만 호출

			 // Released (완료) 액션 바인딩
			// Completed 이벤트: 입력이 해제되었을 때 한 번만 호출
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
	void BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedPtr, ReleasedFuncType ReleasedPtr, HeldFuncType HeldPtr);
};

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
inline void UAuraInputComponent::BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedPtr, ReleasedFuncType ReleasedPtr, HeldFuncType HeldPtr)
{
	check(InputConfig);
	for (const FAuraInputTag& InputAction : InputConfig->AbilityInputActions)
	{
		if (InputAction.InputAction && InputAction.InputTag.IsValid())
		{
			if (PressedPtr)
			{
				//지정 액션에 태그를 바인드 / 콜백함수에게 인자로 Tag를 전달
				BindAction(InputAction.InputAction, ETriggerEvent::Started, Object, PressedPtr, InputAction.InputTag);
			}
			if(ReleasedPtr)
			{
				BindAction(InputAction.InputAction, ETriggerEvent::Completed, Object, ReleasedPtr, InputAction.InputTag);
			}
			if(HeldPtr)
			{
				BindAction(InputAction.InputAction, ETriggerEvent::Triggered, Object, HeldPtr, InputAction.InputTag);
			}
		}
	}
}
