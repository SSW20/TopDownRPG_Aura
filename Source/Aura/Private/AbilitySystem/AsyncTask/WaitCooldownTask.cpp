// Copyright Druid Mechanics


#include "AbilitySystem/AsyncTask/WaitCooldownTask.h"


// 정적 팩토리 함수 구현: 쿨다운 대기 태스크의 인스턴스를 생성하고 초기화
UWaitCooldownTask* UWaitCooldownTask::WaitForCooldownTask(UAbilitySystemComponent* InASC, const FGameplayTag& InTag)
{
    // 새로운 UWaitCooldownTask 인스턴스를 생성
    UWaitCooldownTask* WaitCooldownTask = NewObject<UWaitCooldownTask>();
	
    // 전달받은 어빌리티 시스템 컴포넌트와 쿨다운 태그를 멤버 변수에 저장
    WaitCooldownTask->ASC = InASC;
    WaitCooldownTask->CooldownTag = InTag;
	
    if (!IsValid(InASC) || !InTag.IsValid())
    {
       WaitCooldownTask->EndTask();
       return nullptr; 
    }

    // 쿨다운 태그의 변경(추가/제거)을 감지하기 위해 델리게이트를 등록.
    // EGameplayTagEventType::NewOrRemoved: 태그가 새로 추가되거나 제거될 때만 콜백을 호출.
    WaitCooldownTask->ASC->RegisterGameplayTagEvent(InTag, EGameplayTagEventType::NewOrRemoved).AddUObject(WaitCooldownTask, &UWaitCooldownTask::CooldownChanged);
    
    // ActiveGameplayEffect가 ASC에 추가될 때 감지하기 위해 델리게이트를 등록
    // OnActiveGameplayEffectAddedDelegateToSelf: 자신에게 게임플레이 효과가 추가될 때 호출
    // 이 델리게이트는 쿨다운 효과가 시작될 때를 감지하는 데 사용
    WaitCooldownTask->ASC->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownTask, &UWaitCooldownTask::OnActiveEffectedAdded);
	
    return WaitCooldownTask;
}

// 태스크 종료 및 정리 함수 구현
void UWaitCooldownTask::EndTask()
{
    // ASC가 유효하지 않으면 더 이상 정리할 것이 없으므로 즉시 반환
    if (!IsValid(ASC)) return;

    // 이전에 등록했던 게임플레이 태그 이벤트 델리게이트 바인딩을 모두 제거
    // RemoveAll: 이 객체에 바인딩된 모든 콜백을 제거
    ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);


    // 태스크가 완전히 완료되었음을 시스템에 알립니다.
    // SetReadyToDestroy(): 이 액션이 더 이상 필요 없으며 삭제될 준비가 되었음을 나타냅니다.
    SetReadyToDestroy();
	
    // 가비지 컬렉션 대상이 되도록 마크합니다.
    MarkAsGarbage();
}

// 쿨다운 태그의 카운트가 변경될 때 호출되는 콜백 함수 구현
void UWaitCooldownTask::CooldownChanged(const FGameplayTag Tag, int32 Count)
{
    // 태그 카운트가 0이면 쿨다운이 종료되었음을 의미
	if (Count == 0)
	{
		CooldownEnd.Broadcast(0.f);
	}
}

// ActiveGameplayEffect가 ASC에 추가될 때 호출되는 콜백 함수 구현
void UWaitCooldownTask::OnActiveEffectedAdded(UAbilitySystemComponent* TargetASC,
    const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
    // 대상 ASC가 유효하지 않으면 즉시 반환
    if (!IsValid(TargetASC)) return;
	
	// FGameplayEffectSpec의 모든 에셋 태그를 가져옵니다.
    FGameplayTagContainer AssetTags;
    SpecApplied.GetAllAssetTags(AssetTags);

	// FGameplayEffectSpec의 모든 부여된 태그를 가져옵니다..
    FGameplayTagContainer GrantedTags;
    SpecApplied.GetAllGrantedTags(GrantedTags);

    // AssetTags또는 GrantedTags에 이 태스크가 모니터링하는 CooldownTag가 포함되어 있는지 확인
    if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
    {
       // 쿨다운 태그와 일치하는 모든 소유 태그를 가진 게임플레이 효과를 쿼리하기 위한 쿼리 객체를 생성
       FGameplayEffectQuery EffectQuery = FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(CooldownTag.GetSingleTagContainer());
       
       // ASC에서 쿼리와 일치하는 활성 효과들의 남은 시간을 가져옵니다.
       TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(EffectQuery);
       
       // 남은 시간 배열에 요소가 하나라도 있다면 (쿨다운 효과가 존재한다면)
       if (TimesRemaining.Num() > 0)
       {
          // 가장 긴 남은 시간을 찾기 위한 초기화
          int32 Index = 0;
          float HighestRemaining = TimesRemaining[0]; // 첫 번째 요소를 초기 최댓값으로 설정

          // 배열을 순회하며 가장 긴 남은 시간을 찾습니다.
          for (int32 i = 1; i < TimesRemaining.Num(); i++) // 두 번째 요소부터 시작
          {
             if (TimesRemaining[i] > HighestRemaining)
             {
                HighestRemaining = TimesRemaining[i]; // 더 큰 값을 찾으면 최댓값 갱신
                Index = i; // 해당 인덱스 갱신 (여기서는 사용되지 않지만 일반적인 패턴)
             }
          }

          // CooldownStart 델리게이트를 브로드캐스트하여 쿨다운 시작을 알리고 가장 긴 남은 시간을 전달합니다.
          CooldownStart.Broadcast(HighestRemaining);
       }
    }
	
}
