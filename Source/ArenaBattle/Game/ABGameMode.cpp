// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ABGameMode.h"
//#include"Player/ABPlayerController.h"
#include "Character/ABCharacterPlayer.h"

AABGameMode::AABGameMode()
{
	// 블루 프린트 클래스 애셋 로드
	//static ConstructorHelpers::FClassFinder<APawn> PawnClassRef(
	//	TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C")
	//);

	// 검색 성공 여부 확인
	//if (PawnClassRef.Succeeded())
	//{
	//	// 기본 폰 클래스 설정
	//	DefaultPawnClass = PawnClassRef.Class;
	//}

	// 기본 폰 클래스 설정
	DefaultPawnClass = AABCharacterPlayer::StaticClass();

	// 기본 클래스 지정
	// DefaultPawnClass
	//PlayerControllerClass = AABPlayerController::StaticClass();

	//static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerRef(
	//	TEXT("/Script/ArenaBattle.ABPlayerController")
	//);

	//static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerRef(
	//	TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonPlayerController.BP_ThirdPersonPlayerController_C")
	//);
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerRef(
		TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonPlayerController.BP_ThirdPersonPlayerController_C")
	);

	if (PlayerControllerRef.Succeeded())
	{
		PlayerControllerClass = PlayerControllerRef.Class;
	}
}