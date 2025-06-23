// Copyright Druid Mechanics


#include "Input/AuraInputConfig.h"
#include "InputAction.h"

const UInputAction* UAuraInputConfig::FindInputActionByTag(const FGameplayTag& Tag) const
{
	// const 함수이기 때문에 AbilityInputActions도 함수 내부에서 const로 간주
	for (const FAuraInputTag& InputTag : AbilityInputActions)
	{
		if (InputTag.InputAction && InputTag.InputTag.MatchesTagExact(Tag))
		{
			return InputTag.InputAction;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Can not find Valid Input Action By Tag"));
	return nullptr;
}
