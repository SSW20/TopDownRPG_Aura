// Copyright Druid Mechanics


#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Kismet/GameplayStatics.h"
#include "AuraGameplayTags.h"
#include "Game/AuraGameModeBase.h"
#include "AuraAbilityTypes.h"
#include "Interaction/CombatInterface.h"
#include "UI/HUD/AuraHUD.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContext)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContext, 0))
	{
		if (AAuraHUD* HUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WCParams(PC, PS, ASC, AS );
			return HUD->GetOverlayWidgetController(WCParams);
		}
	}
	return nullptr;


}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuController(const UObject* WorldContext)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContext, 0))
	{
		if (AAuraHUD* HUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WCParams(PC, PS, ASC, AS);
			return HUD->GetAttributeMenuWidgetController(WCParams);
		}
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitDefaultAttributes(const UObject* WorldContext, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	AAuraGameModeBase* AuraGamemode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContext));
	check(AuraGamemode);

	UCharacterClassInfo* CharacterInfo = AuraGamemode->CharacterInfoClass;
	FCharacterClassDefaultInfo CharacterClassDefaultInfo = CharacterInfo->GetCharacterDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle DefaultContextHandle = ASC->MakeEffectContext();
	DefaultContextHandle.AddSourceObject(ASC->GetAvatarActor());
	FGameplayEffectSpecHandle DefaultSpecHandle = ASC->MakeOutgoingSpec(CharacterClassDefaultInfo.DefaultAttributes,Level, DefaultContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*DefaultSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryContextHandle = ASC->MakeEffectContext();
	SecondaryContextHandle.AddSourceObject(ASC->GetAvatarActor());
	FGameplayEffectSpecHandle SecondarySpecHandle = ASC->MakeOutgoingSpec(CharacterInfo->SecondaryAttributes, Level, SecondaryContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondarySpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalContextHandle = ASC->MakeEffectContext();
	VitalContextHandle.AddSourceObject(ASC->GetAvatarActor());
	FGameplayEffectSpecHandle VitalSpecHandle = ASC->MakeOutgoingSpec(CharacterInfo->VitalAttributes, Level, VitalContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalSpecHandle.Data.Get());
}

void UAuraAbilitySystemLibrary::InitDefaultAbilities(const UObject* WorldContext, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	AAuraGameModeBase* AuraGamemode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContext));
	check(AuraGamemode);

	UCharacterClassInfo* CharacterInfo = AuraGamemode->CharacterInfoClass;
	for (auto StartAbility : CharacterInfo->StartAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(StartAbility, 1);
		{
			ASC->GiveAbility(AbilitySpec);
		}
	}

	const FCharacterClassDefaultInfo& ClassInfo = CharacterInfo->GetCharacterDefaultInfo(CharacterClass);
	for (TSubclassOf<UGameplayAbility> ClassAbility : ClassInfo.ClassAbilities)
	{
		ICombatInterface* Enemy = Cast<ICombatInterface>(ASC->GetAvatarActor());
		if (Enemy)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(ClassAbility, Enemy->GetPlayerLevel());
			{
				ASC->GiveAbility(AbilitySpec);
			}
		}
	}
}


	// static_cast는 컴파일러에게 "내가 이 타입으로 캐스팅할 수 있다는 걸 보증하니, 네가 런타임에 이 타입이 맞는지 확인은 하지 마"라고 말하는 것과 같습니다
bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetIsBlocked();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetIsCriticalHit();
	}
	return false;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsBlocked(bInIsBlockedHit);
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsCriticalHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithInRadius(const UObject* WorldContextObject,
    TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin)
{
    // 충돌 쿼리 파라미터 설정
    FCollisionQueryParams SphereParams;
    // 겹침 쿼리에서 무시할 액터들을 추가 (예: 쿼리를 시작한 공격자 자신).
    SphereParams.AddIgnoredActors(ActorsToIgnore);
    
    // 월드 컨텍스트 오브젝트로부터 현재 게임 월드 객체를 가져옵니다.
    // EGetWorldErrorMode::LogAndReturnNull: 월드를 가져오지 못하면 로그를 남기고 nullptr을 반환
    if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
       TArray<FOverlapResult> Overlaps; // 겹침 쿼리 결과를 저장할 배열
       
       // 월드에 오브젝트 타입별로 겹침 쿼리(OverlapMultiByObjectType)를 수행합니다.
       // Overlaps: 겹치는 모든 결과가 저장될 출력 배열.
       // SphereOrigin: 구체 쿼리의 중심 위치.
       // FQuat::Identity: 구체의 회전은 고려하지 않으므로 Identity (기본값) 사용.
       // FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects):
       //   - 쿼리할 오브젝트 타입 설정: 모든 '동적 오브젝트' (물리 시뮬레이션 중이거나 움직이는 액터)를 대상으로 합니다.
       // FCollisionShape::MakeSphere(Radius): 쿼리할 구체 모양과 반경을 정의합니다.
       // SphereParams: 위에서 설정한 쿼리 파라미터 (무시할 액터 목록 포함).
       World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity,
          FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
       
       // 겹침 쿼리 결과(Overlaps) 배열을 순회하며 각 액터를 필터링합니다.
       for (FOverlapResult& Overlap : Overlaps)
       {
	       // 겹친 액터가 CombatInterface를 구현하는지 확인하고, 죽지 않았는지 확인
	       // Implements<UCombatInterface>(): 해당 액터가 특정 인터페이스를 구현하는지 확인
	       if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
	       {
		       // 모든 조건을 만족하면, 겹친 액터를 OutOverlappingActors 배열에 추가합니다.
		       // AddUnique: 배열에 이미 같은 액터가 있다면 추가하지 않고, 없다면 추가합니다 (중복 방지).
		       // ICombatInterface::Execute_GetActor(Overlap.GetActor()): CombatInterface의 GetActor() 함수를 호출하여 실제 액터를 가져옵니다.
		       OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetActor(Overlap.GetActor()));
	       }
       }
    }
}