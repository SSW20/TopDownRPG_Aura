#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "GameplayTagContainer.h"
#include "Interaction/CombatInterface.h"
#include "Net/UnrealNetwork.h"
#include "Character/AuraCharacterBase.h"
#include "AuraGameplayTags.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Interaction/PlayerInterface.h"
#include "Player/AuraPlayerController.h"

UAuraAttributeSet::UAuraAttributeSet()
 {
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);

	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CritHitChance, GetCritHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CritHitResistance, GetCritHitResistAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CritHitDamage, GetCritHitDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegenerate, GetHealthRegenerateAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ManaRegenerate, GetManaRegenerateAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMana, GetMaxManaAttribute);
	
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Lightning, GetLightningResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);
}


void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//특정 속성을 네트워크를 통해 복제하도록 설정하고, 값이 변경될 때 RepNotify 함수를 호출하도록 지정


	//		Primary Attribute
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);


	//		Secondary Attribute
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CritHitChance, COND_None, REPNOTIFY_Always);	
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CritHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CritHitResist, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegenerate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegenerate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	//		Damage Resistance
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);

	//		Vital Attribute
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);

}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	//매크로를 통해 어빌리티 시스템에게 체력 값이 변경되었다는 사실을 알리는 역할
	UE_LOG(LogTemp, Warning, TEXT("Health Changed"));

	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	UE_LOG(LogTemp, Warning, TEXT("Strength Changed"));
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CritHitChance(const FGameplayAttributeData& OldCritChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CritHitChance, OldCritChance);
}

void UAuraAttributeSet::OnRep_CritHitDamage(const FGameplayAttributeData& OldCritDMG) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CritHitDamage, OldCritDMG);
}

void UAuraAttributeSet::OnRep_CritHitResist(const FGameplayAttributeData& OldCritResist) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CritHitResist, OldCritResist);
}

void UAuraAttributeSet::OnRep_HealthRegenerate(const FGameplayAttributeData& OldHealthRegenerate) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegenerate, OldHealthRegenerate);
}

void UAuraAttributeSet::OnRep_ManaRegenerate(const FGameplayAttributeData& OldManaRegenerate) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegenerate, OldManaRegenerate);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldFireResistance);
}

void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightingResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldLightingResistance);
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldArcaneResistance);
}

void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldPhysicalResistance);
}




void UAuraAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue)  const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());

	}
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

		
	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	if (Props.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(Props.TargetCharacter)) return;
	
	// 메타데이터 : IncomingExp, IncomingDamage ==> 이들은 복제 되지 않고 값을 받은 후 바로 0으로 세팅
	if(Data.EvaluatedData.Attribute == GetIncomingExpAttribute())
	{
		HandleIncomingExp(Props);
	}
	
	if(Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Props);
	}
}
void UAuraAttributeSet::HandleIncomingExp(FEffectProperties& Props)
{
	/*
	*	UAuraAbilitySystemLibrary::ApplyDamageEffect에서 FDamageEffectParams을 받고 새로운 FGameplayEffectSpecHandle을 생성 후 대상 액터에게 적용

		UExecCalc_Damage의 DetermineDebuff에서 이 파라미터와 대상의 저항을 바탕으로 디버프 성공 여부와 최종 수치를 계산하고 FAuraGameplayEffectContext에 기록

		NetSerialize를 통해 이 기록된  결과가 네트워크로 동기화

		마지막으로, PostGameplayEffectExecute 에서 HandleDebuff함수에서 활용하는 흐름

		디버프를 적용시키지 않는 CauseDamage 라는 Blueprintcallable이라는 함수도 있지만 쓰지 않을 예정
	 */
	
	float LocalIncomingExp = GetIncomingExp();
	SetIncomingExp(0);
		
	if (Props.SourceCharacter->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddExp(Props.SourceCharacter,LocalIncomingExp);

		//	Level Up 계산 
		int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);
		int32 CurrentExp = IPlayerInterface::Execute_GetExp(Props.SourceCharacter);

		int32 NewLevel = IPlayerInterface::Execute_GetLevelByExp(Props.SourceCharacter, CurrentExp + LocalIncomingExp);
		// Level Up
		if (NewLevel > CurrentLevel)
		{
			int32 AttributePoint = IPlayerInterface::Execute_GetAttributePointReward(Props.SourceCharacter, NewLevel);
			int32 SkillPoint = IPlayerInterface::Execute_GetSkillPointReward(Props.SourceCharacter, NewLevel);

			// Add Attribute Point, Skill Point, Level
			IPlayerInterface::Execute_AddAttributePoint(Props.SourceCharacter, AttributePoint);
			IPlayerInterface::Execute_AddSkillPoint(Props.SourceCharacter, SkillPoint);
			IPlayerInterface::Execute_AddLevel(Props.SourceCharacter, NewLevel - CurrentLevel);
				
			// Update Mana, Health To Max
			bIsMaxHealth = true;
			bIsMaxMana = true;
				
			IPlayerInterface::Execute_LevelUp(Props.SourceCharacter);
		}
			
	}
}

void UAuraAttributeSet::HandleIncomingDamage(FEffectProperties& Props)
{
	const float IncomeDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);
		
	float NewHealth = GetHealth() - IncomeDamage;
	bool bIsDead = false;

	SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

	if (NewHealth <= 0)
		bIsDead = true;
		
	if (!bIsDead)
	{
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(FAuraGameplayTags::Get().Effect_HitReact);
			
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningActor())->TryActivateAbilitiesByTag(TagContainer);

		const FVector& KnockbackForce = UAuraAbilitySystemLibrary::GetKnockbackImpulsVector(Props.EffectContextHandle);
		if (!KnockbackForce.IsNearlyZero(1.f))
		{
			Props.TargetCharacter->LaunchCharacter(KnockbackForce, true, true);
		}
	}
	else
	{
		if (ICombatInterface* Target = Cast<ICombatInterface>(Props.TargetAvatarActor))
		{
			// 죽었을 때 날라가게 만들기 DeathImpulseVector 추가
			Target->Die(UAuraAbilitySystemLibrary::GetDeathImpulsVector(Props.EffectContextHandle));
		}
		SendExp(Props);
			
	}

	if (Props.TargetCharacter != Props.SourceCharacter)
	{
		if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.SourceController))
		{
			const bool bIsBlocked = UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
			const bool bIsCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
			PC->ShowDamageNumber(IncomeDamage, Props.TargetCharacter, bIsBlocked, bIsCriticalHit);
		}
		if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.TargetController))
		{
			const bool bIsBlocked = UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
			const bool bIsCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
			PC->ShowDamageNumber(IncomeDamage, Props.TargetCharacter, bIsBlocked, bIsCriticalHit);
		}

		if (UAuraAbilitySystemLibrary::IsDebuffSuccess(Props.EffectContextHandle))
		{
			// 무엇을 할거냐?
			HandleDebuff(Props);
		}
	}

	
}

void UAuraAttributeSet::HandleDebuff(FEffectProperties& Props)
{
	/*
 		HandleDebuff함수에서는 Debuff가 성공했을 때만 호출 

		이는 실제 디버프를 적용시킬 GameplayEffect를 만들어 Duration, Damage, Period,... 을 적용시킨 뒤 GE를 적용하는 역할을 함 

		(이 GE는 다시 PostGameplayEffectExecute 부르지만 Debuff 성공은 설정하지 않아 무한루프에 빠지지않음)
 	*/
	FGameplayEffectContextHandle EffectContexthandle = Props.SourceASC->MakeEffectContext();
	EffectContexthandle.AddSourceObject(Props.SourceAvatarActor);

	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
	const FGameplayTag DamageTypeTag =  UAuraAbilitySystemLibrary::GetDamageType(Props.EffectContextHandle);
	const float DebuffDamage = UAuraAbilitySystemLibrary::GetDebuffDamage(Props.EffectContextHandle);
	const float DebuffDuration = UAuraAbilitySystemLibrary::GetDebuffDuration(Props.EffectContextHandle);
	const float DebuffFrequency = UAuraAbilitySystemLibrary::GetDebuffFrequency(Props.EffectContextHandle);
	
	
	FString DebuffName = FString::Printf(TEXT("Debuff : %s"),*DamageTypeTag.ToString());
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(),FName(DebuffName));

	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Effect->Period = DebuffFrequency;
	Effect->DurationMagnitude = FScalableFloat(DebuffDuration);
	Effect->bExecutePeriodicEffectOnApplication = false;

	// Effect->InheritableOwnedTagsContainer.AddTag(Tags.DamageTypesToDebuff[DamageTypeTag]); << 버전 업데이트 이후로 쓰지 않음

	FInheritedTagContainer TagContainer = FInheritedTagContainer();
	UTargetTagsGameplayEffectComponent& Component = Effect->FindOrAddComponent<UTargetTagsGameplayEffectComponent>();
	TagContainer.Added.AddTag(Tags.DamageTypesToDebuff[DamageTypeTag]);
	TagContainer.CombinedTags.AddTag(Tags.DamageTypesToDebuff[DamageTypeTag]);
	Component.SetAndApplyTargetTagChanges(TagContainer);

	Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;
	Effect->StackLimitCount = 1;

	int32 Index = Effect->Modifiers.Num();
	Effect->Modifiers.Add(FGameplayModifierInfo());
	FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Index];
	
	ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = GetIncomingDamageAttribute();
	
	FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContexthandle, 1.f);
	if (MutableSpec)
	{
		FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(MutableSpec->GetContext().Get());
		TSharedPtr<FGameplayTag> DebuffDamageTypeTag = MakeShareable(new FGameplayTag(DamageTypeTag));
		AuraEffectContext->SetDamageType(DebuffDamageTypeTag);

		Props.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
	}
	
}


void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	if (Attribute == GetMaxManaAttribute() && bIsMaxMana)
	{
		SetMana(GetMaxMana());
		bIsMaxMana = false;
	}
	if (Attribute == GetMaxHealthAttribute() && bIsMaxHealth)
	{
		SetHealth(GetMaxHealth());
		bIsMaxHealth = false;
	}

}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	// Source = causer of the effect, Target = target of the effect (owner of this AS)

	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

// 실제로 Props의 데이터를 읽어와 GameplayEvent를 보냄 --> 이후 GA_Event_Listen의 WaitGameplayEvent로 연결됨
void UAuraAttributeSet::SendExp(FEffectProperties& Props)
{
	if (Props.TargetCharacter->Implements<UCombatInterface>())
	{
		const ECharacterClass ChracterClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);
		
		int32 Level = 1;
	
		if (Props.SourceCharacter->Implements<UCombatInterface>())
		{
			Level = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);
		}
		const int32 Exp = UAuraAbilitySystemLibrary::GetExpRewardByClassAndLevel(Props.TargetCharacter, ChracterClass, Level);

		const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
		
		FGameplayEventData Payload;
		Payload.EventTag = Tags.Attributes_Meta_IncomingExp;
		Payload.EventMagnitude = Exp;
		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, Tags.Attributes_Meta_IncomingExp, Payload);
	}
}


