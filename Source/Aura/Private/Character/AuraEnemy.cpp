// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraEnemy.h"
#include "Aura/Aura.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "UI/Widget/AuraUserWidget.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AuraGameplayTags.h"
#include "UI/Widget/FloatingDamageText.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(GetRootComponent());
	//HealthBarWidgetComponent->SetRelativeLocation(FVector(0, 0, 30));
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

void AAuraEnemy::HitReactTagChanged(const FGameplayTag Tag, int32 count)
{
	if (count > 0) bIsReacting = true;
	GetCharacterMovement()->MaxWalkSpeed = bIsReacting ? 0.f : MoveSpeed;
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;



	UAuraUserWidget* HealthWidget = Cast<UAuraUserWidget>(HealthBarWidgetComponent->GetWidget());
	HealthWidget->SetWidgetController(this);


	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).
		AddLambda([this](const FOnAttributeChangeData& Data) {OnHealthChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).
		AddLambda([this](const FOnAttributeChangeData& Data) {OnMaxHealthChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Effect_HitReact, EGameplayTagEventType::NewOrRemoved).
		AddUObject(this,&AAuraEnemy::HitReactTagChanged);


	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
}

void AAuraEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	InitializeDefaultAttributes();
}

void AAuraEnemy::InitializeDefaultAttributes() const 
{
	UAuraAbilitySystemLibrary::InitDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
	UAuraAbilitySystemLibrary::InitDefaultAbilities(this, AbilitySystemComponent);
}

void AAuraEnemy::Die()
{
	Super::Die();
	SetLifeSpan(LifeSpan);
}
