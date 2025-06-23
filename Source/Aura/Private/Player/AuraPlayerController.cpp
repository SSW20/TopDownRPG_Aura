// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Input/AuraInputComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SplineComponent.h"
#include "Interaction/EnemyInterface.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "AuraGameplayTags.h"
AAuraPlayerController::AAuraPlayerController()
{
	//서버에서 값 변경시 클라이언트도 복제되서 바뀜
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>(FName("Spine"));
}
void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	AutoRun();
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);

	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();

	/*
	*		액터 라인 트레이스 상황 
	* A. LastActor == null && ThisActor == null
	*		- 아무것도 안함
	* B. LastActor == null && ThisActor != null
	*		- Highlight ThisActor
	* C. LastActor != null && ThisActor == null
	*		- UnHighlight LastActor
	* D. LastActor != null && ThisActor != null
	*		D.1 LastActor != ThisActor 
	*			- UnHighlight LastActor + Highlight ThisActor
	*		D.2 LastActor == ThisActor
	*			- 아무것도 안함
	*/

	if (LastActor == nullptr)
	{
		if (ThisActor == nullptr)
		{
			//	상황 A 
			//	아무것도 안함
		}
		else
		{
			// 상황 B
			// Highlight ThisActor
			ThisActor->HighlightActor();
		}
	}
	else
	{
		if (ThisActor == nullptr)
		{
			//	상황 C 
			//	UnHighlight LastActor
			LastActor->UnHighlightActor();
		}
		else 
		{
			if (ThisActor != LastActor)
			{
				//상황 D.1
				//UnHighlight LastActor + Highlight ThisActor
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			else
			{
				//상황 D.2
				//아무것도 안함
			}
		}
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, FString::Printf(TEXT("Pressed: %s"), *InputTag.ToString()));
	if(FAuraGameplayTags::Get().InputTag_LMB.MatchesTagExact(InputTag))
	{
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!FAuraGameplayTags::Get().InputTag_LMB.MatchesTagExact(InputTag))
	{
		GetASC()->PlayIfReleased(InputTag);
		return;
	}
	if (bTargeting || bIsShiftPressed)
	{
		GetASC()->PlayIfReleased(InputTag);
	}
	else
	{
		APawn* PlayerPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && PlayerPawn)
		{
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, PlayerPawn->GetActorLocation(), CachedDestination))
			{
				Spline->ClearSplinePoints();
				for (auto& Path : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(Path, ESplineCoordinateSpace::World);
				}
				CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
			}
			bAutoRunning = true;
			FollowTime = 0.f;
		}
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!FAuraGameplayTags::Get().InputTag_LMB.MatchesTagExact(InputTag))
	{
		GetASC()->PlayIfHeld(InputTag);
		return;
	}
	if (bTargeting || bIsShiftPressed)
	{
		GetASC()->PlayIfHeld(InputTag);
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();
		FHitResult HitResult;
		if (GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult))
		{
			CachedDestination = HitResult.ImpactPoint;
		}
		if (APawn* PlayerPawn = GetPawn())
		{
			FVector MoveDestination = CachedDestination - PlayerPawn->GetActorLocation();
			PlayerPawn->AddMovementInput(MoveDestination);
		}
	}

}

void AAuraPlayerController::ShiftReleased()
{
	bIsShiftPressed = false;
}

void AAuraPlayerController::ShiftPressed()
{
	bIsShiftPressed = true;
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (ASC == nullptr)
	{
		ASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return ASC;
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning)
		return;
	if (APawn* PlayerPawn = GetPawn())
	{
		const FVector ClosestLocactionOnSpline = Spline->FindLocationClosestToWorldLocation(PlayerPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		FVector ClosetDirectionOnSpline = Spline->FindDirectionClosestToWorldLocation(PlayerPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		PlayerPawn->AddMovementInput(ClosetDirectionOnSpline);

		float DistanceFromDestination = (CachedDestination - ClosestLocactionOnSpline).Length();
		if (DistanceFromDestination <= AutoRunAcceptanceRadius)
			bAutoRunning = false;
 	}
}



void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//Check --> 매크로, 조건이 실패하면 크래쉬남
	check(AuraContext);

	//싱글톤 패턴으로 이루어져 있음
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// 매핑 컨텍스트, 우선순위
		Subsystem->AddMappingContext(AuraContext, 0);
	}
	// 마우스 커서 설정
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	// 게임&UI 모드 설정
	FInputModeGameAndUI InputModeData;											//게임 + UI 입력 모두 가능하게 설정, FInputModeGameOnly이 기본 -> 마우스 커서 X 
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);	//마우스가 화면 밖으로 나갈 수 있게 만듦
	InputModeData.SetHideCursorDuringCapture(false);							//마우스가 클릭 시 숨겨지지 않음 
	SetInputMode(InputModeData);												//모드 적용
}

//어떤 키를 눌렀을 때 어떤 행동을 할지를 정해주는 입력 설정 담당자
//무슨 키/버튼을 눌렀을 때 (입력 이벤트) 어떤 행동 이름표(InputAction) 를 실행하고 실제로 어떤 함수를 호출해서 게임에서 반응할지를 미리 약속 해두는 곳
void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//CastChecked : Cast + Check
	// 플레이어 컨트롤러에는 InputComponent라는 변수가 있음
	//InputComponent는 UInputComponent 타입의 포인터이지만, 실제로는 UEnhancedInputComponent 타입의 객체의 주소를 저장하고 있기 때문에 캐스트함
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &AAuraPlayerController::AbilityInputTagPressed, &AAuraPlayerController::AbilityInputTagReleased, &AAuraPlayerController::AbilityInputTagHeld);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	// 좌우 회전값 == Yaw 값만 남김
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	// 카메라를 고정시켰기 때문에 그냥 월드 좌표계에서 하는 게 더 편함
	//PlayerStart를 회전 시키지 않았을 때 --> 동일 
	//PlayerStart를 회전시켰을 때 --> 위의 방법은 캐릭터의 방향을 기준으로 움직이고, 아래 방법은 월드좌표계 기준으로 움직임
	
	//const FRotator ControllerRotation = GetControlRotation();
	//const FRotator YawRotation = FRotator(0, ControllerRotation.Yaw, 0);

	////회전 행렬 (3x3) 을 생성 ==> 방향 변환 지도 생성, 기존 정면방향 (1,0,0)에서 얼마나 회전되어있는지 적혀있음
	//const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // 회전된 X축의 방향 단위 벡터를 추출, 열 추출, 게임에서의 정면 방향
	//const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);   // 회전된 Y축의 방향 단위 벡터를 추출, 열 추출, 게임에서의 오른쪽 방향


	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		//ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		//ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);

		ControlledPawn->AddMovementInput(FVector::ForwardVector, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(FVector::RightVector, InputAxisVector.X);
	}


}

