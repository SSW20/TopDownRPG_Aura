// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);

}

// Spell Point를 눌렀을 때 일어나는 로직
void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	// Skill Point를 사용
	// 이는 다시 SpellMenuWidget의 SkillPointChangeDelegate을 부른다
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddSkillPoint(GetAvatarActor(),-1);
	}

	// Selected Globe의 Ability Tag를 통해 Spec을 가져옴
	// 이후 Spec을 통해 Status Tag를 가져옴
	//		1. Unlocked 이거나 Equipped일시 Level UP
	//		2. Eligible일시 Unlocked으로 변경
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		FAuraGameplayTags Tags = FAuraGameplayTags::Get();
		FGameplayTag StatusTag = GetStatusTagFromSpec(*AbilitySpec);

		if (StatusTag.MatchesTagExact(Tags.Abilities_Status_Eligible))
		{
			AbilitySpec->DynamicAbilityTags.RemoveTag(Tags.Abilities_Status_Eligible);
			AbilitySpec->DynamicAbilityTags.AddTag(Tags.Abilities_Status_Unlocked);
		}
		else if (StatusTag.MatchesTagExact(Tags.Abilities_Status_Unlocked) || StatusTag.MatchesTag(Tags.Abilities_Status_Equipped))
		{
			AbilitySpec->Level += 1;
		}

		// 이후 Ability의 Status가 변경되었으니 이 함수를 호출
		// 이는 다시 SpellMenuWidgetController의 AbilityStatusTagsDelegate를 호출
		ClientUpdateStatus_Implementation(AbilityTag,Tags.Abilities_Status_Unlocked, AbilitySpec->Level);
	}

	
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle EffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bIsStartUpAbilitiesBroadCasted)
	{
		AbilitiesGivenDelegate.Broadcast();
	}
}

bool UAuraAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription,
	FString& OutNextLevelDescription)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if(UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))
		{
			OutDescription = AuraAbility->GetDescription(AbilitySpec->Level);
			OutNextLevelDescription = AuraAbility->GetNextLevelDescription(AbilitySpec->Level + 1);
			return true;
		}
	}
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (AbilityTag.IsValid())
	{
		OutDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoByTag(AbilityTag).LevelRequired);

	}
	else
	{
		OutNextLevelDescription = FString();
	}
	return false;
}

void UAuraAbilitySystemComponent::ClientUpdateStatus_Implementation(const FGameplayTag& AbilityTag,
                                                                    const FGameplayTag& StatusTag, int32 Level)
{
	AbilityStatusTagsDelegate.Broadcast(AbilityTag, StatusTag, Level);
}

void UAuraAbilitySystemComponent::AddGameplayAbilities(const TArray<TSubclassOf<UGameplayAbility>>& GameplayAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : GameplayAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UAuraGameplayAbility* GA = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(GA->StartTag);
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}
	bIsStartUpAbilitiesBroadCasted = true;
	AbilitiesGivenDelegate.Broadcast();
}

// StartUP 의 Passive Ability를 설정해줌 --> EX ) 경험치를 받는 어빌리티
void UAuraAbilitySystemComponent::AddPassiveGameplayAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& PassiveGameplayAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : PassiveGameplayAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

//활성화 가능한 모든 어빌리티 스펙을 순회하며 주어진 델리게이트를 실행하는 함수
void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& ForEachDelegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (ForEachDelegate.IsBound())
		{
			ForEachDelegate.ExecuteIfBound(AbilitySpec);
		}
		else
		{
			UE_LOG(LogAura, Error, TEXT("Failed To execute delegate"));
		}
	}
}

// Ability Info 에서 Ability를 찾고 이미 부여된 Ability라면 무시
void UAuraAbilitySystemComponent::UpdateAbilityStatus(int32 Level)
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInfos)
	{
		if (!IsValid(Info.Ability) || !IsValid(Info.Ability)) continue;
		//해당 Ability의 레벨을 플레이어가 넘었는지 확인
		if (Info.LevelRequired > Level) continue;
		if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);
			GiveAbility(AbilitySpec);
			// Ability Spec 강제 복제
			MarkAbilitySpecDirty(AbilitySpec);

			// UpdateAbilityStatus가 서버에서 실행되니 클라에서도 실행되게 만듦
			ClientUpdateStatus_Implementation(Info.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible, AbilitySpec.Level);
		}
	}
}

//이미 부여된 Ability라면 해당 Spec을 반환 
FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& Tag)
{
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (const FGameplayTag& AbilityTag : AbilitySpec.Ability->AbilityTags)
		{
			if (AbilityTag.MatchesTag(Tag))
			{
				return &AbilitySpec;
			}
		}
	}
	return nullptr;
}
// GameplayAbilitySpec을 통해 Input Tag를 가져옴
FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& Spec)
{
	for (FGameplayTag Tag : Spec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("InputTag")))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

// GameplayAbilitySpec을 통해 Ability Tag를 가져옴
FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& Spec)
{
	for (FGameplayTag Tag : Spec.Ability.Get()->AbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("Abilities")))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}
// Spec으로 부터 StatusTag를 반환
FGameplayTag UAuraAbilitySystemComponent::GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag StatusTag : AbilitySpec.DynamicAbilityTags)
	{
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
		{
			return StatusTag;
		}
	}
	return FGameplayTag();
}
void UAuraAbilitySystemComponent::PlayIfHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::PlayIfReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::UpgradeAttributes(const FGameplayTag& Tag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		// Attribute Point가 있을 시 서버 호출 --> 불필요한 함수 호출 없앰
		if (IPlayerInterface::Execute_GetAttributePoint(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(Tag);
		}
	}
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& Tag)
{
	FGameplayEventData Payload;
	Payload.EventTag = Tag;
	Payload.EventMagnitude = 1.f;

	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddAttributePoint(GetAvatarActor(),-1);
	}
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), Tag, Payload);
}
