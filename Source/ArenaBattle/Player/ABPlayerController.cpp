// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ABPlayerController.h"
#include "UI/ABHUDWidget.h"

AABPlayerController::AABPlayerController()
{
	// 프로젝트에서 클래스 정보 가져오기
	static ConstructorHelpers::FClassFinder<UABHUDWidget> ABHUDWidgetRef(
		TEXT("/Game/ArenaBattle/UI/WBP_ABHUD.WBP_ABHUD_C")
	);

	if (ABHUDWidgetRef.Succeeded())
	{
		ABHUDWidgetClass = ABHUDWidgetRef.Class;
	}	
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 입력 모드 설정.
	// 게임 시작되면 뷰포트로 바로 입력 되도록.
	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
	
	// 위젯 생성
	ABHUDWidget = CreateWidget<UABHUDWidget>(this, ABHUDWidgetClass);
	
	// 위젯 객체가 잘 생성됐으면, 화면에 추가
	if (ABHUDWidget != nullptr)
	{
		// 화면에 추가해 UI가 보일 수 있도록 설정
		ABHUDWidget->AddToViewport();
	}
}
