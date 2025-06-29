#include "AuraAbilityTypes.h"

bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	uint32 RepBits = 0;

	// 현재 아카이브가 '저장' (데이터를 네트워크로 보낼) 모드일 때만 실행됩니다.
	if (Ar.IsSaving())
	{
		if (bReplicateInstigator && Instigator.IsValid())
		{
			RepBits |= 1 << 0;
		}
		if (bReplicateEffectCauser && EffectCauser.IsValid() )
		{
			RepBits |= 1 << 1;
		}
		if (AbilityCDO.IsValid())
		{
			RepBits |= 1 << 2;
		}
		if (bReplicateSourceObject && SourceObject.IsValid())
		{
			RepBits |= 1 << 3;
		}
		if (Actors.Num() > 0)
		{
			RepBits |= 1 << 4;
		}
		if (HitResult.IsValid())
		{
			RepBits |= 1 << 5;
		}
		if (bHasWorldOrigin)
		{
			RepBits |= 1 << 6;
		}
		if (bIsCriticalHit)
		{
			RepBits |= 1 << 7;
		}
		if (bIsBlocked)
		{
			RepBits |= 1 << 8;
		}
	}


	//	RepBits를 네트워크로 전송.
	//  받는 쪽(Loading 모드)에서 이 지도를 먼저 읽어 어떤 데이터가 뒤따라올지 알 수 있음.
	Ar.SerializeBits(&RepBits, 9);

	
	// 역 직렬화란 ?
	// Ar가 '불러오기(Loading)' 모드일 때, 이 << 연산자는 Ar 안에 있는 데이터 스트림의 현재 위치에서 Instigator 변수에 맞는 크기와 형식의
	// 데이터를 '읽어서'(꺼내서) Instigator 변수에 채워 넣는 작업을 수행.
	//읽기가 끝나면 Ar의 내부 포인터는 읽은 데이터만큼 다음 위치로 이동
	
	if (RepBits & (1 << 0))
	{
		Ar << Instigator;
	}
	if (RepBits & (1 << 1))
	{
		Ar << EffectCauser;
	}
	if (RepBits & (1 << 2))
	{
		Ar << AbilityCDO;
	}
	if (RepBits & (1 << 3))
	{
		Ar << SourceObject;
	}
	if (RepBits & (1 << 4))
	{
		SafeNetSerializeTArray_Default<31>(Ar, Actors);
	}
	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = TSharedPtr<FHitResult>(new FHitResult());
			}
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}
	if (RepBits & (1 << 7))
	{
		Ar << bIsCriticalHit;
	}
	if (RepBits & (1 << 8))
	{
		Ar << bIsBlocked;
	}


	//'불러오기' 모드일 때만 실행
	// Instigator와 EffectCauser는 이미 역직렬화되었으므로 (Ar을 통해), 이를 사용하여
	// InstigatorAbilitySystemComponent와 같은 파생된 필드들을 다시 초기화.
	// 이 필드들은 직접 직렬화되지 않고 다른 필드에 의해 초기화되었기 떄문.
	if (Ar.IsLoading())
	{
		//// InstigatorAbilitySystemComponent를 초기화하기 위함
		AddInstigator(Instigator.Get(), EffectCauser.Get()); 
	}	
	
	bOutSuccess = true;
	return true;
}
