// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Input/AuraInputConfig.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;
class UAuraAbilitySystemComponent;
/**
 * 
 */

UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;
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

	bool bIsShiftPressed = false;

	void Move(const  FInputActionValue& InputActionValue);

	void CursorTrace();

	TScriptInterface<IEnemyInterface> LastActor;
	TScriptInterface<IEnemyInterface> ThisActor;

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

	FVector CachedDestination = FVector::Zero();
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	bool bTargeting = false;
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;
	TObjectPtr<class USplineComponent> Spline;

	void AutoRun();
};
