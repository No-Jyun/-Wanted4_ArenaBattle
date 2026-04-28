// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_Detect.h"

#include "ABAI.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/OverlapResult.h"
#include "Interface/ABCharacterAIInterface.h"
#include "Physics/ABCollision.h"

UBTService_Detect::UBTService_Detect()
{
	// 노드 이름 설정
	NodeName = TEXT("Detect");

	// 서비스 실행 간격 설정 (단위 : 초)
	Interval = 1.0f;
}

void UBTService_Detect::TickNode(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// 구현하고 싶은 것 : AI폰 위치를 기준으로 특정 반경 안에 캐릭터가 있는지 확인
	// 1. AI 폰 위치 가져오기
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return;
	}

	// 폰 위치 저장
	FVector Center = ControllingPawn->GetActorLocation();

	// 2. 캐릭터 감지 - 충돌 (오버랩) 기능을 활용해 캐릭터 감지
	UWorld* World = ControllingPawn->GetWorld();
	if (!World)
	{
		return;
	}

	// 인터페이스로 형변환
	IABCharacterAIInterface* AIPawn = Cast<IABCharacterAIInterface>(ControllingPawn);
	if (!AIPawn)
	{
		return;
	}

	// NPC가 캐릭터를 감지할 범위 (반지름)
	float DetectRadius = AIPawn->GetAIDetectRange();

	// 쿼리 파라미터
	FCollisionQueryParams Params(
		SCENE_QUERY_STAT(Detect),
		false,
		ControllingPawn
	);

	TArray<FOverlapResult> Results;
	bool bResult = World->OverlapMultiByChannel(
		Results,
		Center,
		FQuat::Identity,
		CCHANNEL_ABACTION,
		FCollisionShape::MakeSphere(DetectRadius),
		Params
	);

	// 3. 감지된 정보는 블랙보드에 기록
	// 디버그 용도로 감지 영역 시각화
	if (bResult)
	{
		// 감지된 결과 처리
		for (const auto& OverlapResult : Results)
		{
			// 감지된 액터를 폰으로 변환
			APawn* DetectedPawn = Cast<APawn>(OverlapResult.GetActor());

			// 플레이어인지 확인
			if (DetectedPawn && DetectedPawn->GetController()->IsPlayerController())
			{
				// 감지한 정보를 블랙보드에 저장
				OwnerComp.GetBlackboardComponent()->
				          SetValueAsObject(BBKEY_TARGET, DetectedPawn);

				// 감지 영역 시각화 (디버그 용도)
				DrawDebugSphere(
					World,
					Center,
					DetectRadius,
					16,
					FColor::Green,
					false,
					0.2f
				);

				DrawDebugPoint(
					World,
					DetectedPawn->GetActorLocation(),
					10.0f,
					FColor::Green,
					false,
					0.2f
				);

				// NPC에서 감지된 폰으로 선긋기
				DrawDebugLine(
					World,
					Center,
					DetectedPawn->GetActorLocation(),
					FColor::Green,
					false,
					0.2f
				);

				return;
			}
		}
	}

	// 감지 못한 경우는 타겟 액터를 null로 저장
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGET, nullptr);

	// 감지 영역을 빨간색으로 표시 (디버그 용도)
	DrawDebugSphere(
		World,
		Center,
		DetectRadius,
		16,
		FColor::Red,
		false,
		0.2f
	);
}
