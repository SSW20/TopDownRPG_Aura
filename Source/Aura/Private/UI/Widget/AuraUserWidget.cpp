// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AuraUserWidget.h"

void UAuraUserWidget::SetWidgetController(UObject* Object)
{
	WidgetController = Object;
	WidgetControllerSet();
}

