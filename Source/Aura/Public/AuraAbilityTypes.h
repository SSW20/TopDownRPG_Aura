#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"		// USTRUCT 사용을 위한 자동 생성 헤더


USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY();
public:
	bool GetIsCriticalHit() const { return bIsCriticalHit; }
	bool GetIsBlocked() const { return bIsBlocked; }
	void SetIsCriticalHit(bool bNewVal) { bIsCriticalHit = bNewVal; }
	void SetIsBlocked(bool bNewVal) { bIsBlocked = bNewVal; }
	
	// 리플렉션 시스템용 UScriptStruct 반환 
	// 언리얼 엔진이 런타임에 USTRUCT 객체의 정확한 실제 타입을 질의하고 확인할 수 있도록 하는 핵심적인 메커니즘
	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const
	{
		return FGameplayEffectContext::StaticStruct();
	}

	/** Custom serialization, subclasses must override this */
	// 네트워크 직렬화/역직렬화 처리
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	// FGameplayEffectContext에 이미 virtual Duplicate() 함수가 있으므로, 오버라이드
	virtual FAuraGameplayEffectContext* Duplicate() const
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();

		// 현재 객체의 모든 멤버를 새 객체로 복사 (WithCopy=true가 이를 보장)
		*NewContext = *this;
		if (GetHitResult())
		{
			NewContext->AddHitResult(*GetHitResult(), true);
		}

		// 새 컨텍스트 포인터 반환
		return NewContext;
	}
protected:
	UPROPERTY()
	bool bIsCriticalHit = false;
	UPROPERTY()
	bool bIsBlocked = false;
};

// 커스텀 FGameplayEffectContext 구조체를 네트워크 복제 및 복사 가능하도록 완벽하게 설정하기 위해 필요
// TStructOpsTypeTraits 타입의 특별한 구조체
// FAuraGameplayEffectContext에 대한 특성을 언리얼 엔진에 알려주는 템플릿 특수화
template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		// NetSerialize() 함수를 가지고 있으며 네트워크 직렬화가 가능함을 명시
		WithNetSerializer = true, 
		// 복사 연산(Copy/Duplicate)이 가능함을 명시
		WithCopy = true 
	};
};