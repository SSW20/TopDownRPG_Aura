// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Input/AuraInputConfig.h"
#include "GameplayTagContainer.h"
#include "NiagaraSystem.h"
#include "Actor/MagicCircle.h"
#include "Interaction/HighlightInterface.h"
#include "AuraPlayerController.generated.h"

class UFloatingDamageText;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;
class UAuraAbilitySystemComponent;
/**
 * 
 */
// TargetingEnemy : 적 조준
// TargetingNonEnemy : 적이 아닌 것 (포탈, 체크포인트 등)을 조준
// NotTargeting : 조준하고 있지 않음
enum class ETargetingStatus : uint8
{
	TargetingEnemy,
	TargetingNonEnemy,
	NotTargeting
};


UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

//UFUNCTION(Client, Reliable):
// -Client: 이 함수는 '클라이언트 RPC'입니다.서버에서만 호출할 수 있으며,
//* 해당 PlayerController를 소유한 '특정 클라이언트' 머신에서만 실행됩니다.
//* (모든 클라이언트가 아닌, 특정 플레이어의 화면에 UI를 띄울 때 사용)
//* -Reliable : 이 RPC 호출은 네트워크를 통해 대상 클라이언트에게 '반드시 전달되어 실행됨'을 보장합니다.
//* (데미지 숫자는 중요한 피드백이므로 누락되어서는 안 됩니다.)
	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float Damage, ACharacter* TargetCharacter, bool bIsBlocked, bool bIsCritHit);

	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* Material = nullptr);

	UFUNCTION(BlueprintCallable)
	void HideMagicCircle();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	
private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ShiftAction;
	
	void MagicCircleTrace();

	bool bIsShiftPressed = false;

	void Move(const  FInputActionValue& InputActionValue);

	void CursorTrace();

	TObjectPtr<AActor> LastActor;
	TObjectPtr<AActor> ThisActor;

	static void HighlightActor(AActor* InActor);
	static void UnHighlightActor(AActor* InActor);

	ETargetingStatus TargetingStatus = ETargetingStatus::NotTargeting;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	void ShiftReleased();
	void ShiftPressed();

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> ASC;

	UAuraAbilitySystemComponent* GetASC();

	/*
	*	FVector CachedDestination: 마우스 클릭 지점의 월드 위치를 캐시(저장)할 변수.
		float FollowTime: 마우스 왼쪽 버튼을 누르고 있는 시간을 추적할 변수.
		float ShortPressThreshold: 'Short Press'를 판단하는 시간 임계값.
		bool bAutoRunning: 캐릭터가 자동으로 목적지로 이동 중인지 나타내는 플래그.
		bool bTargeting : 마우스가 이동인지 적을 타겟팅하는 건지 나타내는 플래그
		float AutoRunAcceptanceRadius: 자동 이동 시 목적지에 얼마나 가까워져야 멈출지 결정하는 반경.
		TObjectPtr<class USplineComponent> Spline: 경로를 부드럽게 만들 스플라인 컴포넌트 포인터.
	*/

	FHitResult CursorHit;
	FVector CachedDestination = FVector::Zero();
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	bool bTargeting = false;
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;
	TObjectPtr<class USplineComponent> Spline;

	void AutoRun();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFloatingDamageText> DamageTextClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMagicCircle> MagicCircleDecalClass;
	
	UPROPERTY()
	TObjectPtr<AMagicCircle> MagicCircle;
};
