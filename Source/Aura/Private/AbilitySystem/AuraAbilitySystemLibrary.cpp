// Copyright Druid Mechanics


#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Kismet/GameplayStatics.h"
#include "AuraGameplayTags.h"
#include "Game/AuraGameModeBase.h"
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
