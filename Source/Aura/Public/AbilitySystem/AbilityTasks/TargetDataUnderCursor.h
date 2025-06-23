
#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderCursor.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetDataSignature, const FGameplayAbilityTargetDataHandle&, Data);


UCLASS()
class AURA_API UTargetDataUnderCursor : public UAbilityTask
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "TargetDataUnderMouse", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UTargetDataUnderCursor* GetTargetDataUnderMouseCursor(UGameplayAbility* OwningAbility);

	UPROPERTY(BlueprintAssignable)
	FTargetDataSignature ValidData;

private:
	virtual void Activate() override;
	void SendCursorData();
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);
};
