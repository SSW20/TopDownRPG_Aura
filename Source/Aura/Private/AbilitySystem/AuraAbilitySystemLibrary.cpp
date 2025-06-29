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

void UAuraAbilitySystemLibrary::InitDefaultAbilities(const UObject* WorldContext, UAbilitySystemComponent* ASC)
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