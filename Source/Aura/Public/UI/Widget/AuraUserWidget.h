// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* object);
	
protected:
	//위젯 컨트롤러가 설정 되었다고 알리는 역할 Begin Play와 비슷
	//BlueprintImplementableEvent : 바디 구현 불가능 , 블루프린트에서 커스텀 이벤트 역할
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
	
};
