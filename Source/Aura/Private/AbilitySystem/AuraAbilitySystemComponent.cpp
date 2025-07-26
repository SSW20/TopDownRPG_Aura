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

void UAuraAbilitySystemComponent::EffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle EffectHandle)
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

// SlotInputTag == Equip버튼을 누른 후 클릭한 Globe의 input Tag, AbilityTag == Equip버튼을 누르기 전 Ability. 즉 장착할 어빌리티
void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& SlotInputTag, const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		// 장착할 어빌리티의 PrevSlotTag
		const FGameplayTag& PrevSlotTag = GetInputTagFromSpec(*AbilitySpec);
		const FGameplayTag& Status = GetStatusTagFromSpec(*AbilitySpec);

		const bool bStatusValid = Status == FAuraGameplayTags::Get().Abilities_Status_Equipped || Status == FAuraGameplayTags::Get().Abilities_Status_Unlocked;
		if (bStatusValid)
		{
			// Slot이 비어있는지 아닌지 확인 하는 함수 
			if (!SlotIsEmpty(SlotInputTag))
			{
				// 슬롯이 비어있지 않다면 
				FGameplayAbilitySpec* PrevAbilitySpec = GetSpecWithSlot(SlotInputTag);
				if (PrevAbilitySpec)
				{
					const FGameplayTag PrevAbilityTag = GetAbilityTagFromSpec(*PrevAbilitySpec);

					// 만약 선택한 Ability와 전에 있던 Ability의 태그가 같다면 그냥 리턴
					if (PrevAbilityTag.MatchesTagExact(AbilityTag))
					{
						ClientEquipAbility(AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Equipped,SlotInputTag, PrevSlotTag);	
						return;
					}
					// 패시브인지 아닌지 확인
					if (IsPassiveAbility(*PrevAbilitySpec))
					{
						// 슬롯에 있는 Passive Ability를 DeActive
						DeactivatePassiveTagDelegate.Broadcast(GetAbilityTagFromSpec(*PrevAbilitySpec));
						MulticastActivatePassiveEffect(PrevAbilityTag, false);
					}

					// 해당 Input Tag에 있는 Spec을 지움
					ClearInputTagBySpec(PrevAbilitySpec);
				}
			}

			// 이전에 있던 어빌리티를 Clear 후

			if (!AbilityHasInputTag(*AbilitySpec))
			{
				// 만약 신규 어빌리티가 Input Tag를 가지고 있지 않다면

				// 패시브일시
				if (IsPassiveAbility(*AbilitySpec))
				{
					// Activate
					TryActivateAbility(AbilitySpec->Handle);
					MulticastActivatePassiveEffect(AbilityTag, true);
				}
			}
			AssignSlotToAbility(*AbilitySpec, SlotInputTag);
			MarkAbilitySpecDirty(*AbilitySpec);
		
			//	이 로직의 문제점 --> 이전 슬롯의 어빌리티는 무엇인가? 모름 / 이전 슬롯이 Passive일시 DeActivate할 수 있나? 없음
			//
			// 
			// // slot Input Tag를 가지고 있는 모든 액티브 어빌리티 청소 
			// ClearAbilitiesFromInputTag(SlotInputTag);
			//
			// // 이제 AbilityTag로 Spec을 가져와
			// // Spec에 Inputtag지우고, Status바꿔
			// // 새로운 InputTag넣어주고 끝
			// ClearInputTagBySpec(AbilitySpec);
			// AbilitySpec->DynamicAbilityTags.AddTag(SlotInputTag);
			//
			// if (GetStatusTagFromSpec(*AbilitySpec).MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Unlocked))
			// {
			// 	AbilitySpec->DynamicAbilityTags.RemoveTag(FAuraGameplayTags::Get().Abilities_Status_Unlocked);
			// 	AbilitySpec->DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
			// }
		
		
			ClientEquipAbility(AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Equipped,SlotInputTag, PrevSlotTag);
		}
	}
	
	// Spec도 없다 --> 그냥 무시하죠?
}

void UAuraAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& PassiveTag, bool bActive)
{
	ActivatePassiveDelegate.Broadcast(PassiveTag, bActive);
}

// Spec을 DynamicAbilityTags에서 찾음
FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecWithSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(Slot))
		{
			return &AbilitySpec;
		}
	}
	return nullptr;
}

// Ability에 InputTag Add
void UAuraAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	ClearInputTagBySpec(&Spec);
	Spec.DynamicAbilityTags.AddTag(Slot);
}

// AbilityTag로 Ability를 찾아 Description 수정
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

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& StatusTag, const FGameplayTag& InputSlotTag, const FGameplayTag& PrevInputSlotTag)
{
	AbilityInfoTagsDelegate.Broadcast(AbilityTag, StatusTag, InputSlotTag, PrevInputSlotTag);
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

bool UAuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& Spec)
{
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	const FGameplayTag AbilityTag = GetAbilityTagFromSpec(Spec);
	const FAuraAbilityInfo& Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
	const FGameplayTag AbilityType = Info.AbilityType;
	return AbilityType.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Type_Passive);
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
	FScopedAbilityListLock ActiveScopeLock(*this);
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

// InputTag로 식별
FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromInputTag(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (const FGameplayTag& AbilityTag : AbilitySpec.Ability->AbilityTags)
		{
			if (AbilityTag.MatchesTagExact(InputTag))
			{
				return &AbilitySpec;
			}
		}
	}
	return nullptr;
}

// 활성화된 Ability들을 돌면서 특정 InputTag가 있으면 지움
void UAuraAbilitySystemComponent::ClearAbilitiesFromInputTag(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (HasInputTag(AbilitySpec, InputTag))
		{
			ClearInputTagBySpec(&AbilitySpec);
		}
	}
}

// Spec을 가지고 Spec의 inputTag를 삭제
void UAuraAbilitySystemComponent::ClearInputTagBySpec(FGameplayAbilitySpec* AbilitySpec)
{
	if (AbilitySpec)
	{
		const FGameplayTag InputTag = GetInputTagFromSpec(*AbilitySpec);
		AbilitySpec->DynamicAbilityTags.RemoveTag(InputTag);
	}
}

// 해당 Spec에 특정 inputTag가 있는지 확인
bool UAuraAbilitySystemComponent::HasInputTag(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& InputTag)
{
	for (const FGameplayTag& AbilityTag : AbilitySpec.DynamicAbilityTags)
	{
		if (AbilityTag.MatchesTag(InputTag))
		{
			return true;
		}
	}
	return false;
}

// Input Tag를 가지는지 확인  
bool UAuraAbilitySystemComponent::AbilityHasInputTag(const FGameplayAbilitySpec& AbilitySpec)
{
	return AbilitySpec.DynamicAbilityTags.HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag")));
}

void UAuraAbilitySystemComponent::PlayIfPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
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
	if(Spec.Ability)
	{
		for(FGameplayTag Tag: Spec.Ability.Get()->AbilityTags)
		{
			if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
	// for (FGameplayTag Tag : Spec.Ability.Get()->AbilityTags)
	// {
	// 	if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("Abilities")))
	// 	{
	// 		return Tag;
	// 	}
	// }
	// return FGameplayTag();
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

// AbilityTag를 통해 Spec을 얻은 뒤 해당 Spec의 StatusTag를 반환
FGameplayTag UAuraAbilitySystemComponent::GetStatusTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag);
	if (AbilitySpec)
	{
		return GetStatusTagFromSpec(*AbilitySpec);
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag);
	if (AbilitySpec)
	{
		return GetInputTagFromSpec(*AbilitySpec);
	}
	return FGameplayTag();
}

bool UAuraAbilitySystemComponent::SlotIsEmpty(const FGameplayTag& SlotTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (HasInputTag(AbilitySpec, SlotTag))
		{
			return false;
		}
	}
	return true;
}

void UAuraAbilitySystemComponent::PlayIfHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	FScopedAbilityListLock ActiveScopeLock(*this);
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
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			AbilitySpecInputReleased(AbilitySpec);
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
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
