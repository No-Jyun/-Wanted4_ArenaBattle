// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterNonPlayer.h"

AABCharacterNonPlayer::AABCharacterNonPlayer()
{

}

void AABCharacterNonPlayer::SetDead()
{
	// 상위 로직 실행
	Super::SetDead();

	// 타이머를 사용해 일정 시간 대기 후 액터 제거
	// 레퍼런스 (&) LValue Reference (참조) : LValue (자리를 차지해야함) 를 참조함
	// 람다 구문
	// [] -> 캡처 (Capture) - 람다 함수 본문에서 사용할 정보를 설정
	// () -> 파라미터 선언
	// -> 리턴 구문
	// { } 본분
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle, 
		FTimerDelegate::CreateLambda(
			[&]()
			{
				Destroy();
			}
		),
		DeadEventDelayTime,
		false
	);
}
