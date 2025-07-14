// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "WaitCooldownTask.generated.h"

/**
 * 이 클래스는 특정 게임플레이 태그(쿨다운 태그)에 대한 쿨다운 시작 및 종료를 비동기적으로 대기하는 블루프린트 비동기 태스크입니다.
 * UI에서 쿨다운 시각화를 위해 사용됩니다.
 */

// 쿨다운 시그니처 델리게이트 선언.
// float 타입의 'TimeRemaining' 매개변수 하나를 브로드캐스트하는 동적 멀티캐스트 델리게이트
// DYNAMIC_MULTICAST: 블루프린트에서 이벤트 디스패처로 노출될 수 있으며, 여러 함수를 바인딩할 수 있습니다.
// _OneParam: 하나의 매개변수를 가집니다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownSignature, float, TimeRemaining);

UCLASS()
class AURA_API UWaitCooldownTask : public UBlueprintAsyncActionBase
{
    GENERATED_BODY() // 언리얼 엔진 리플렉션 시스템을 위한 필수 매크로

public:
    // 쿨다운이 시작될 때 브로드캐스트되는 델리게이트
    UPROPERTY(BlueprintAssignable)
    FCooldownSignature CooldownStart;
    
    // 쿨다운이 종료될 때 브로드캐스트되는 델리게이트
    UPROPERTY(BlueprintAssignable)
    FCooldownSignature CooldownEnd;

    // 정적 팩토리 함수
    // UFUNCTION(BlueprintCallable): 블루프린트에서 호출할 수 있는 함수로 만듭니다.
    // meta = (BlueprintInternalUseOnly = "true"): 이 함수가 블루프린트 노드 자체를 생성하는 내부 함수임을 나타냅니다. 따라서 블루프린트 컨텍스트 메뉴에서 "Wait For Cooldown Task" 노드로 나타납니다.
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
    static UWaitCooldownTask* WaitForCooldownTask(UAbilitySystemComponent* ASC, const FGameplayTag& Tag);
    
    // 이 비동기 태스크를 종료하고 모든 리소스를 정리하는 함수
    UFUNCTION(BlueprintCallable)
    void EndTask();

protected:
    // TObjectPtr: UObject에 대한 스마트 포인터로, 가비지 컬렉션에 의해 관리됩니다.
    TObjectPtr<UAbilitySystemComponent> ASC;

    // 특정 쿨다운 게임플레이 태그
    FGameplayTag CooldownTag;

    // 쿨다운 태그의 카운트가 변경될 때 호출되는 콜백 함수입니다.
    void CooldownChanged(const FGameplayTag Tag, int32 Count);

    // ActiveGameplayEffect가 ASC에 추가될 때 호출되는 콜백 함수입니다.
    void OnActiveEffectedAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);
};
