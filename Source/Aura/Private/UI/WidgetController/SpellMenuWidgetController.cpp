// Copyright Druid Mechanics


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	PlayerSkillPointChangedDelegate.Broadcast(GetAuraPlayerState()->GetSkillPoint());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	// 둘 다 레벨업 시 호출
	// 두 개의 시나리오
	// 1. AbilityInfoDelegate가 먼저 호출 --> SpellGlobeSelected 에는 Skill Point가 아직 업데이트 되지 않아 잘못된 결과
	// 2. PlayerSkillPointChangedDelegate가 먼저 호출 --> SpellGlobeSelected의 Status Tag가 업데이트 되지 않아 잘못된 결과
	// 해결방법 --> broadcast하기 전에 local 변수로 SkillPoint와 Status를 추적하고 이후 broadcast
	GetAuraAbilitySystemComponent()->AbilityStatusTagsDelegate.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 Level)
	{
		// 실시간으로 버튼 Enable을 설정하기 위해 다시 한번 BroadCast 
		if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
		{
			bool bEnableSpendPoints = false;
			bool bEnableEquip = false;
			ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);

			FString Description;
			FString NextLevelDescription;
			GetAuraAbilitySystemComponent()->GetDescriptionsByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
			SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
		}
		
		if (AbilityInfo)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
			Info.AbilityTag = AbilityTag;
			Info.StatusTag = StatusTag;	
			AbilityInfoDelegate.Broadcast(Info);
		}
	});

	GetAuraPlayerState()->SkillPointChangeDelegate.AddLambda([this](int32 SkillPoint)
	{
		PlayerSkillPointChangedDelegate.Broadcast(SkillPoint);
		CurrentSpellPoints = SkillPoint;

		// 실시간으로 버튼 Enable을 설정하기 위해 다시 한번 BroadCast 
		bool bEnableSpendPoints = false;
		bool bEnableEquip = false;
		ShouldEnableButtons(SelectedAbility.Status, SkillPoint, bEnableSpendPoints, bEnableEquip);
		
		FString Description;
		FString NextLevelDescription;
		GetAuraAbilitySystemComponent()->GetDescriptionsByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
		SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
	});

	GetAuraAbilitySystemComponent()->AbilityInfoTagsDelegate.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);
}

// 버튼을 누른 Globe의 정보를 저장
void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitingForEquip)
	{
		const FGameplayTag TypeTag = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.Ability).AbilityType;
		StopWaitEquipSelectDelegate.Broadcast(TypeTag);
		bWaitingForEquip =false;
	}
	
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();	
	const int32 SpellPoints = GetAuraPlayerState()->GetSkillPoint();
	FGameplayTag AbilityStatus;	
	
	
	const FGameplayAbilitySpec* AbilitySpec = GetAuraAbilitySystemComponent()->GetSpecFromAbilityTag(AbilityTag);
	if (!AbilityTag.IsValid()  || AbilitySpec == nullptr)
	{
		AbilityStatus = GameplayTags.Abilities_Status_Locked;
	}
	else
	{
		AbilityStatus = GetAuraAbilitySystemComponent()->GetStatusTagFromSpec(*AbilitySpec);
	}

	SelectedAbility.Ability = AbilityTag;
	SelectedAbility.Status = AbilityStatus;
	
	bool bEnableSpendPoints = false;
	bool bEnableEquip = false;
	FString Description;
	FString NextLevelDescription;
	
	GetAuraAbilitySystemComponent()->GetDescriptionsByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
	ShouldEnableButtons(AbilityStatus, SpellPoints, bEnableSpendPoints, bEnableEquip);
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
	
}

// Spell Point 버튼을 누를 시 함수 호출, SelectedAbility의 Ability를 넘겨준다
void USpellMenuWidgetController::PressedSpellPointButton()
{
	GetAuraAbilitySystemComponent()->ServerSpendSpellPoint(SelectedAbility.Ability);
}

void USpellMenuWidgetController::DeselectGlobe()
{
	// Equip Animation이 실행중일 때 없어지도록
	if (bWaitingForEquip)
	{
		const FGameplayTag TypeTag = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.Ability).AbilityType;
		StopWaitEquipSelectDelegate.Broadcast(TypeTag);
		bWaitingForEquip =false;
	}
	SelectedAbility.Ability = FGameplayTag();
	SelectedAbility.Status = FGameplayTag();
	SpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	const FGameplayTag TypeTag = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.Ability).AbilityType;
	if (TypeTag.IsValid() && !bWaitingForEquip)
	{
		WaitEquipSelectDelegate.Broadcast(TypeTag);
		bWaitingForEquip = true;
	}

	// 선택했던 Globe의 AbilityType을 판단하고 Equipped라면 Input Tag를 저장
	if (SelectedAbility.Status.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
	{
		SelectedSlot = AuraAbilitySystemComponent->GetInputFromAbilityTag(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::EquipGlobePressed(const FGameplayTag& SlotInputTag, const FGameplayTag& AbilityType)
{
	if (!bWaitingForEquip) return;
	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.Ability).AbilityType;
	if (!SelectedAbilityType.IsValid() || !SelectedAbilityType.MatchesTagExact(AbilityType)) return;

	AuraAbilitySystemComponent->ServerEquipAbility(SlotInputTag, SelectedAbility.Ability);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,
	const FGameplayTag& InputSlotTag, const FGameplayTag& PrevInputSlotTag)
{
	FAuraAbilityInfo PrevInfo;
	PrevInfo.InputTag = PrevInputSlotTag;
	PrevInfo.StatusTag = FAuraGameplayTags::Get().Abilities_Status_Unlocked;
	PrevInfo.AbilityTag = FGameplayTag();
	AbilityInfoDelegate.Broadcast(PrevInfo);

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
	Info.InputTag = InputSlotTag;
	Info.StatusTag = StatusTag;
	AbilityInfoDelegate.Broadcast(Info);

	StopWaitEquipSelectDelegate.Broadcast(AbilityInfo->FindAbilityInfoByTag(AbilityTag).AbilityType);
	bWaitingForEquip = false;
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& StatusTag, int32 SpellPoints, bool& bOutSpellPointsButton, bool& bOutEquipButton)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

	bOutSpellPointsButton = false;
	bOutEquipButton = false;

	
	if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
		bOutEquipButton = true;
		if (SpellPoints > 0)
		{
			bOutSpellPointsButton = true;
		}
	}
	if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		if (SpellPoints > 0)
		{
			bOutSpellPointsButton = true;
		}
	}

	
}
