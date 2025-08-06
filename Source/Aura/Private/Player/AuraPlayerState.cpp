// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"
AAuraPlayerState::AAuraPlayerState()
{
	//서버가 클라이언트를 업데이트하려고 시도하는 빈도
	//플레이어 스테이트에 대한 서버 측 변경 사항이 발생하면 서버는 모든 클라이언트에 업데이트를 보내 서버와 동기화할 수 있도록 합니다.
	//복제되어야 하는 모든 변수는 업데이트되고 서버는 가능하다면 이 NetUpdateFrequency를 충족하려고 시도함

	//어빌리티 시스템 컴포넌트와 어트리뷰트 세트를 플레이어 스테이트에 배치할 것이라면, 이 값을 더 빠르게 업데이트되도록 설정\
	//100이면 초당 100번을 의미
	NetUpdateFrequency = 100.f;


	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, Exp);
	DOREPLIFETIME(AAuraPlayerState, AttributePoint);
	DOREPLIFETIME(AAuraPlayerState, SkillPoint);
}

//  Level 이 바뀌거나 Exp가 바뀌는 것을 모두 이 함수들로 구현 --> 이후 OverlayWidgetController의 콜백함수로 바인드 됨
void AAuraPlayerState::SetLevel(int32 NewLevel)
{
	Level = NewLevel;
	LevelChangeDelegate.Broadcast(Level, true);
}

void AAuraPlayerState::AddToLevel(int32 NewLevel)
{
	Level += NewLevel;
	LevelChangeDelegate.Broadcast(Level, true);
}

void AAuraPlayerState::SetLevelWithLoad(int32 NewLevel)
{
	Level = NewLevel;
	LevelChangeDelegate.Broadcast(Level, false);
}

void AAuraPlayerState::SetExp(int32 NewExp)
{
	Exp = NewExp;
	ExpChangeDelegate.Broadcast(Exp);
}

void AAuraPlayerState::AddToExp(int32 NewExp)
{
	Exp += NewExp;
	ExpChangeDelegate.Broadcast(Exp);
}

void AAuraPlayerState::AddAttributePoint(int32 NewAttributePoint)
{
	AttributePoint += NewAttributePoint;
	AttributePointChangeDelegate.Broadcast(AttributePoint);
}

void AAuraPlayerState::SetAttributePoint(int32 NewAttributePoint)
{
	AttributePoint = NewAttributePoint;
	AttributePointChangeDelegate.Broadcast(AttributePoint);
}

void AAuraPlayerState::SetSkillPoint(int32 NewSkillPoint)
{
	SkillPoint = NewSkillPoint;
	SkillPointChangeDelegate.Broadcast(SkillPoint);
}

void AAuraPlayerState::AddSkillPoint(int32 NewSkillPoint)
{
	SkillPoint += NewSkillPoint;
	SkillPointChangeDelegate.Broadcast(SkillPoint);
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	LevelChangeDelegate.Broadcast(Level, true);
}

void AAuraPlayerState::OnRep_Exp(int32 OldExp)
{
	ExpChangeDelegate.Broadcast(Exp);
}

void AAuraPlayerState::OnRep_AttributePoint(int32 OldAttributePoint)
{
	AttributePointChangeDelegate.Broadcast(OldAttributePoint);
}

void AAuraPlayerState::OnRep_SkillPoint(int32 OldSkillPoint)
{
	SkillPointChangeDelegate.Broadcast(OldSkillPoint);
}
