// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraEnemy.h"
#include "Aura/Aura.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");


}
void AAuraEnemy::HighlightActor()
{
	//포스트 프로세싱 볼륨 : 그림판 전체에 적용하는 필터
	//						최종 렌더링된 화면의 픽셀 색상을 받아서, 우리가 정의한 Material의 로직에 따라 색상을 변형하거나 새로운 효과를 추가
	//스텐실 버퍼 : 어떤 픽셀에 마스크를 씌울지를 결정하는 정보
	//커스텀 뎁스 활성화 + 스텐실 값 할당 : 커스텀 뎁스를 활성화한 오브젝트에게 우리가 원하는 특정 스텐실 버퍼 값 (예: 250) 을 할당
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAuraEnemy::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

int32 AAuraEnemy::GetPlayerLevel() const
{
	return Level;
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeDefaultAttributes();
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
}
