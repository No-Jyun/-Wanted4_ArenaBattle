// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterNonPlayer.h"
#include "Engine/AssetManager.h"
#include "AI/ABAIController.h"

AABCharacterNonPlayer::AABCharacterNonPlayer()
{
	// 시작할 때는 메시 안 보이게 설정
	GetMesh()->SetHiddenInGame(true);

	// AIController 클래스 설정
	AIControllerClass = AABAIController::StaticClass();
	
	// 맵에서 로드 또는 런타임에 스폰(생성)되는 모든 경우
	// 미리 지정한 AIController에 빙의되도록 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AABCharacterNonPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// 예외 처리
	ensureAlways(NPCMeshes.Num() > 0);
	
	// 랜덤으로 인덱스 선택
	int32 RandIndex = FMath::RandRange(0, NPCMeshes.Num() - 1);
	
	// 비동기 방식으로 애셋 로딩
	NPCMeshHandle =	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		NPCMeshes[RandIndex],
		FStreamableDelegate::CreateUObject(
			this,
			&AABCharacterNonPlayer::NPCMeshLoadCompleted
		)
	);
}

void AABCharacterNonPlayer::NPCMeshLoadCompleted()
{
	// 로드가 완료되어 핸들이 유효한지 확인
	if (NPCMeshHandle.IsValid())
	{
		// 로드된 애셋을 스켈레탈 메시로 변환
		USkeletalMesh* NPCMesh = Cast<USkeletalMesh>(NPCMeshHandle->GetLoadedAsset());
		
		// 메시 애셋 설정
		if (NPCMesh)
		{
			// 스켈레탈 메시 애셋 설정
			GetMesh()->SetSkeletalMesh(NPCMesh);
			
			// 메시 컴포넌트가 화면에 보이도록 설정
			GetMesh()->SetHiddenInGame(false);
		}
	}
	
	// 애셋 로드에 사용했던 핸들 해제
	NPCMeshHandle->ReleaseHandle();
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

float AABCharacterNonPlayer::GetAIPatrolRadius()
{
	return 800.0f;
}

float AABCharacterNonPlayer::GetAIDetectRange()
{
	return 400.0f;
}

float AABCharacterNonPlayer::GetAIAttackRange()
{
	return 0.0f;
}

float AABCharacterNonPlayer::GetAITurnSpeed()
{
	return 0.0f;
}
