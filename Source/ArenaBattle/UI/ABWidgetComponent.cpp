// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABWidgetComponent.h"
#include "UI/ABUserWidget.h"

void UABWidgetComponent::InitWidget()
{
	Super::InitWidget();
	
	// Super::InitWidget() 상위 로직을 따라가보면,
	// 함수 실행 과정에서 CreateWidget을 통해 Widget이 생성됨
	// 그 이후에 여기가 실행되므로 Widget 초기화를 보장 받을 수 있음
	UABUserWidget* ABUserWidget = Cast<UABUserWidget>(GetWidget());
	if (ABUserWidget)
	{
		ABUserWidget->SetOwningActor(GetOwner());
	}
	
}
