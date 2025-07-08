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
#include "AI/AuraAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

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

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	
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
void AAuraEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AAuraEnemy::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}
void AAuraEnemy::HitReactTagChanged(const FGameplayTag Tag, int32 count)
{
	if (count > 0) {bIsReacting = true;}
	else bIsReacting = false;
	GetCharacterMovement()->MaxWalkSpeed = bIsReacting ? 0.f : MoveSpeed;
	if (AIController && AIController->GetBlackboardComponent())
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bIsReacting);
	}
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (HasAuthority())
	{
		AIController = Cast<AAuraAIController>(NewController);
		AIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		AIController->RunBehaviorTree(BehaviorTree);
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);
	}
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

	if (HasAuthority())
	{
		UAuraAbilitySystemLibrary::InitDefaultAbilities(this, AbilitySystemComponent, CharacterClass);
	}
	UAuraUserWidget* HealthWidget = Cast<UAuraUserWidget>(HealthBarWidgetComponent->GetWidget());
	HealthWidget->SetWidgetController(this);


	/*UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	if (AuraAttributeSet == nullptr)
	{
		return;
	}*/

	// 3. 가져온 AttributeSet이 NULL인지 확인
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: AttributeSetBase is NULL for %s!"), *GetActorLabel());
		return; // AttributeSet이 없으면 더 진행할 수 없습니다.
	}

	// 4. UAuraAttributeSet*으로 안전하게 캐스팅 시도
	UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet);

	// 5. 캐스팅이 성공했는지 확인
	if (!AuraAttributeSet)
	{
		// 캐스팅 실패! 어떤 타입인지 로그로 확인하여 원인 파악
		UE_LOG(LogTemp, Error, TEXT("ERROR: Cast to UAuraAttributeSet FAILED for %s. Actual AttributeSet type: %s"),
			*GetActorLabel(), *AttributeSet->GetClass()->GetName());
		return; // 올바른 타입의 AttributeSet이 아니므로 더 진행할 수 없습니다.
	}

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

	if (HasAuthority())
	{
		InitializeDefaultAttributes();
	}
}


void AAuraEnemy::InitializeDefaultAttributes() const 
{
	UAuraAbilitySystemLibrary::InitDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);

}

void AAuraEnemy::Die()
{
	SetLifeSpan(LifeSpan);
	if(AIController) AIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
	Super::Die();

}
