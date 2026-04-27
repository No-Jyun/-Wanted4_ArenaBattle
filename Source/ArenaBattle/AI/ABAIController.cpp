// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/ABAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"

AABAIController::AABAIController()
{
	// 사용할 블랙보드 애셋 로드
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAssetRef(
		TEXT("/Game/ArenaBattle/AI/BB_ABCharacter.BB_ABCharacter")	
	);
	
	if (BBAssetRef.Succeeded())
	{
		BBAsset = BBAssetRef.Object;
	}
	
	// 사용할 비헤이비어 트리 애셋 로드
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAssetRef(
		TEXT("/Game/ArenaBattle/AI/BT_ABCharacter.BT_ABCharacter")	
	);
	
	if (BTAssetRef.Succeeded())
	{
		BTAsset = BTAssetRef.Object;
	}
}

void AABAIController::RunAI()
{
	// 블랙보드 컴포넌트 포인터 가져오기
	UBlackboardComponent* BlackBoardPtr = Blackboard.Get();
	
	// 사용할 블랙보드 지정
	// 두 번째 파라미터가 *& 타입이어서 명시적인 변수가 있어야함
	if (UseBlackboard(BBAsset, BlackBoardPtr))
	{
		// 비헤이비어 트리 실행
		bool Result = RunBehaviorTree(BTAsset);
		
		// 예외 처리
		ensureAlways(Result);
	}
}

void AABAIController::StopAI()
{
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	
	if (BTComponent)
	{
		BTComponent->StopTree();
	}
}

void AABAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	// 폰에 빙의하면 AI 로직 실행
	RunAI();
}
