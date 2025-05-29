// Copyright Druid Mechanics


#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Kismet/GameplayStatics.h"
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
