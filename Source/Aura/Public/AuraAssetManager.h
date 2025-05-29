// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AuraAssetManager.generated.h"

/**
 * 프로젝트의 에셋 관리를 담당하는 사용자 정의 에셋 매니저 클래스
 *
 *	게임의 주요 에셋 로딩 및 초기화 작업을 중앙 집중식으로 관리하며,
 *	싱글톤 패턴을 통해 프로젝트 내에서 유일한 인스턴스를 제공
 *	이 클래스를 통해 게임 시작 시 필요한 핵심 데이터나 시스템을 초기화할 수 있습니다.
 */
UCLASS()
class AURA_API UAuraAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static UAuraAssetManager& Get();
protected:
	//이 함수를 통해 게임 시작 시 필요한 초기 데이터 로딩, 시스템 초기화 등의 작업을 수행
	virtual void StartInitialLoading() override;

	
};
