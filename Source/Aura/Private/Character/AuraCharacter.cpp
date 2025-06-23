// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerController.h"
#include "UI/HUD/AuraHUD.h"
AAuraCharacter::AAuraCharacter()
{
	// 움직임 같은 것은 최대한 BeginPlay에 넣자 --> Null이 아니더라도 초기화가 진행중에 덮어씌워질 수 있다.
	// BeginPlay는 모든 컴포넌트의 초기화가 진행된 후에 호출됨
	// 아니면 블루프린트 상에서 설정과 CPP에서의 설정이 같은지 확인 해보자

	GetCharacterMovement()->bOrientRotationToMovement = true;					//캐릭터가 이동할 때 자동으로 이동 방향을 향하도록 회전
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);			//캐릭터가 이동 방향으로 회전할 때의 회전 속도를 결정
	GetCharacterMovement()->bConstrainToPlane = true;							//캐릭터의 이동이 특정 평면으로 제한
	GetCharacterMovement()->bSnapToPlaneAtStart = true;							//게임 시작 시 평면 제약이 활성화된 경우 해당 평면에 즉시 스냅(즉시 달라붙음)
		
	//컨트롤러 입력에 따른 Yaw/Pitch/Roll을 막음
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}
void AAuraCharacter::BeginPlay()
{
    Super::BeginPlay();

	//GetCharacterMovement()->bOrientRotationToMovement = true;
	//GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	//GetCharacterMovement()->bConstrainToPlane = true;
	//GetCharacterMovement()->bSnapToPlaneAtStart = true;
 //   

 //   bUseControllerRotationPitch = false;
 //   bUseControllerRotationYaw = false;
 //   bUseControllerRotationRoll = false;
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();


	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	InitializeDefaultAttributes();
}

//폰이 새로운 컨트롤러에 의해 점유될 때 서버에서 호출되는 함수
void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
	AddGameplayAbilities();
}

//APawn 클래스에 정의된 RepNotify 함수
//RepNotify 함수는 특정 변수가 서버에서 변경되어 클라이언트로 복제될 때 자동으로 호출되는 함수
void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
}

int32 AAuraCharacter::GetPlayerLevel() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}



