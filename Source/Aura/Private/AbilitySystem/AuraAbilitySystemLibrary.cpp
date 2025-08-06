// Copyright Druid Mechanics


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
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

bool UAuraAbilitySystemLibrary::GetWidgetControllerParams(const UObject* WorldContext, FWidgetControllerParams& OutWidgetControllerParams, AAuraHUD*& OutAuraHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContext, 0))
	{
		OutAuraHUD = Cast<AAuraHUD>(PC->GetHUD());
		if (OutAuraHUD)
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			
			OutWidgetControllerParams.PlayerState = PS;
			OutWidgetControllerParams.PlayerController = PC;
			OutWidgetControllerParams.AbilitySystemComponent = ASC;
			OutWidgetControllerParams.AttributeSet = AS;
			
			return true;
		}
	}
	return false;
}
UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContext)
{
	FWidgetControllerParams WidgetControllerParams;
	AAuraHUD* HUD;
	if (GetWidgetControllerParams(WorldContext, WidgetControllerParams, HUD))
	{
		return HUD->GetOverlayWidgetController(WidgetControllerParams);
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuController(const UObject* WorldContext)
{
	FWidgetControllerParams WidgetControllerParams;
	AAuraHUD* HUD;
	if (GetWidgetControllerParams(WorldContext, WidgetControllerParams, HUD))
	{
		return HUD->GetAttributeMenuWidgetController(WidgetControllerParams);
	}
	return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContext)
{
	FWidgetControllerParams WidgetControllerParams;
	AAuraHUD* HUD;
	if (GetWidgetControllerParams(WorldContext, WidgetControllerParams, HUD))
	{
		return HUD->GetSpellMenuWidgetController(WidgetControllerParams);
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
void UAuraAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ULoadScreenSaveGame* SaveGame)
{
	AAuraGameModeBase* AuraGamemode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	check(AuraGamemode);

	UCharacterClassInfo* CharacterInfo = AuraGamemode->CharacterInfoClass;
	if (CharacterInfo == nullptr) return;

	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

	const AActor* SourceAvatarActor = ASC->GetAvatarActor();
	FGameplayEffectContextHandle EffectContexthandle = ASC->MakeEffectContext();
	EffectContexthandle.AddSourceObject(SourceAvatarActor);

	const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(CharacterInfo->PrimaryAttribute_SetByCaller, 1.f, EffectContexthandle);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Primary_Strength, SaveGame->Strength);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Primary_Intelligence, SaveGame->Intelligence);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Primary_Resilience, SaveGame->Resilience);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Primary_Vigor, SaveGame->Vigor);
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
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
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(ClassAbility, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
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

void UAuraAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors,
	TArray<AActor*>& OutClosestTargets, const FVector& Origin)
{
	if (MaxTargets >= Actors.Num())
	{
		OutClosestTargets = Actors;
		return;
	}

	TArray<AActor*> ActorsToCheck = Actors;
	
	while (OutClosestTargets.Num() < MaxTargets)
	{
		float ClosestDistance = FLT_MAX;
		AActor* ClosestTarget = nullptr;
		for (AActor* Actor : ActorsToCheck)
		{
			float Distance = FVector::Dist(Actor->GetActorLocation(),Origin);
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestTarget = Actor;
			}
		}
		if (ClosestTarget)
		{
			OutClosestTargets.AddUnique(ClosestTarget);
			ActorsToCheck.Remove(ClosestTarget);
		}
	}
}

bool UAuraAbilitySystemLibrary::IsFriend(AActor* FirstActor, AActor* SecondActor)
{
	bool bIsPlayerFriend = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	bool bIsEnemyFriend = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));

	return bIsPlayerFriend || bIsEnemyFriend;
}

// CharacterClass 와 Level을 받으면 그에 맞는 ScalableFloat인 EXP를 받음
int32 UAuraAbilitySystemLibrary::GetExpRewardByClassAndLevel(const UObject* WorldContext, ECharacterClass CharacterClass, float Level)
{
	AAuraGameModeBase* AuraGamemode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContext));
	check(AuraGamemode);

	UCharacterClassInfo* CharacterInfo = AuraGamemode->CharacterInfoClass;
	const FCharacterClassDefaultInfo& ClassInfo = CharacterInfo->GetCharacterDefaultInfo(CharacterClass);

	int32 ClassExp = static_cast<int32>(ClassInfo.ExpReward.GetValueAtLevel(Level));
	return ClassExp;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContext)
{
	AAuraGameModeBase* AuraGamemode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContext));
	check(AuraGamemode);
	return AuraGamemode->AbilityInfo;
}

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& Params)
{
	const AActor* SourceActor = Params.SourceAbilitySystemComponent->GetAvatarActor();
	FGameplayEffectContextHandle ContextHandle = Params.SourceAbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(SourceActor);
	SetDeathImpulseVector(ContextHandle, Params.DeathImpulseVector);
	SetKnockImpulseVector(ContextHandle,Params.KnockbackImpulseVector);
	
	SetIsRadialDamage(ContextHandle, Params.bIsRadialDamage);
	SetRadialDamageInnerRadius(ContextHandle, Params.InnerRadialDamage);
	SetRadialDamageOuterRadius(ContextHandle, Params.OuterRadialDamage);
	SetRadialDamageOrigin(ContextHandle, Params.RadialOrigin);

	FGameplayEffectSpecHandle SpecHandle = Params.SourceAbilitySystemComponent->MakeOutgoingSpec(Params.DamageGameplayEffectClass, Params.Level,ContextHandle);
	FAuraGameplayTags Tags = FAuraGameplayTags::Get();
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Params.DamagTypeTag, Params.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Tags.Debuff_Chance, Params.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Tags.Debuff_Duration, Params.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Tags.Debuff_Frequency, Params.DebuffFrequency);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Tags.Debuff_Damage, Params.DebuffDamage);
	
	

	
	Params.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());


	return ContextHandle;
}

TArray<FRotator> UAuraAbilitySystemLibrary::GetEvenlySpreadRotators(const FVector& Forward, const FVector& Axis,
	float Spread, float ProjectileCount)
{
	TArray<FRotator> Rotators;
	if (ProjectileCount > 1)
	{
		const FVector SpreadL = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
		// 양쪽으로 퍼질 수 있게
		const float DeltaSpread = Spread < 360.0f ? Spread / (ProjectileCount - 1) : 360.0f / ProjectileCount;
		for (int i = 0; i < ProjectileCount; i++)
		{
			const FVector Direction = SpreadL.RotateAngleAxis(i * DeltaSpread, Axis);
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		Rotators.Add(Forward.Rotation());
	}
	return Rotators;
}

TArray<FVector> UAuraAbilitySystemLibrary::GetEvenlySpreadVectors(const FVector& Forward, const FVector& Axis,
	float Spread, float ProjectileCount)
{
	TArray<FVector> Rotators;
	if (ProjectileCount > 1)
	{
		const FVector SpreadL = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
		// 양쪽으로 퍼질 수 있게
		const float DeltaSpread = Spread / (ProjectileCount - 1);
		for (int i = 0; i < ProjectileCount; i++)
		{
			const FVector Direction = SpreadL.RotateAngleAxis(i * DeltaSpread, Axis);
			Rotators.Add(Direction);
		}
	}
	else
	{
		Rotators.Add(Forward);
	}
	return Rotators;
}


bool UAuraAbilitySystemLibrary::IsDebuffSuccess(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{ 
		return AuraEffectContext->GetIsDebuffSucceess();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (AuraEffectContext->GetDamageTypeTag().IsValid())
		{
			return *AuraEffectContext->GetDamageTypeTag();
		}
	}
	return FGameplayTag();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulsVector(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDeathImpulseVector();
	}
	return FVector::ZeroVector;
}

FVector UAuraAbilitySystemLibrary::GetKnockbackImpulsVector(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetKnockbackImpulseVector();
	}
	return FVector::ZeroVector;
}

void UAuraAbilitySystemLibrary::SetIsDebuffSuccess(FGameplayEffectContextHandle& EffectContextHandle,
                                                   bool bInSuccessfulDebuff)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsDebuffSucceess(bInSuccessfulDebuff);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float InDamage)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffDamage(InDamage);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float InDuration)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffDuration(InDuration);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, float InFrequency)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffFrequency(InFrequency);
	}
}

void UAuraAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle,
	const FGameplayTag& InDamageType)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		AuraEffectContext->SetDamageType(DamageType);
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulseVector(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InVector)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDeathImpulseVector(InVector);
	}
}

void UAuraAbilitySystemLibrary::SetKnockImpulseVector(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InVector)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetKnockbackImpulseVector(InVector);
	}
}

void UAuraAbilitySystemLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsRadialDamage)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsRadialDamage(bInIsRadialDamage);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle& EffectContextHandle,
	float InInnerRadius)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetInnerRadialDamage(InInnerRadius);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle& EffectContextHandle,
	float InOuterRadius)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetOuterRadialDamage(InOuterRadius);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOrigin(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InOrigin)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetRadialOrigin(InOrigin);
	}
}

bool UAuraAbilitySystemLibrary::IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetIsRadialDamage();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetInnerRadialDamage();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetOuterRadialDamage();
	}
	return 0.f;
}

FVector UAuraAbilitySystemLibrary::GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetRadialOrigin();
	}
	return FVector::ZeroVector;
}