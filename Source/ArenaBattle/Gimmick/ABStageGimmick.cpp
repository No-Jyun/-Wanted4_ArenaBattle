// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/ABStageGimmick.h"

#include "MovieSceneSequenceID.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Physics/ABCollision.h"

// Sets default values
AABStageGimmick::AABStageGimmick()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 컴포넌트 생성
	Stage = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stage"));
	RootComponent = Stage;

	// 메시 애셋 로드 및 설정
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StageMeshRef(
		TEXT("/Game/ArenaBattle/Environment/Stages/SM_SQUARE.SM_SQUARE")
	);
	if (StageMeshRef.Succeeded())
	{
		Stage->SetStaticMesh(StageMeshRef.Object);
	}

	StageTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("StageTrigger"));
	// 박스 컴포넌트 충돌 영역 크기 설정
	StageTrigger->SetBoxExtent(FVector(775.0f, 775.0f, 300.0f));
	// 계층 설정
	StageTrigger->SetupAttachment(Stage);
	StageTrigger->SetRelativeLocation(FVector(0.0f, 0.0f, 250.0f));
	StageTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	StageTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnStageTriggerBeginOverlap);

	// 문 컴포넌트
	static FName GateSockets[] =
	{
		TEXT("+XGate"),
		TEXT("-XGate"),
		TEXT("+YGate"),
		TEXT("-YGate"),
	};
	// 문 메시 로드
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GateMeshRef(
		TEXT("/Game/ArenaBattle/Environment/Props/SM_GATE.SM_GATE")
	);

	// 배열 순회하면서 문 메시 생성.
	for (FName GateSocket : GateSockets)
	{
		// 컴포넌트 생성
		UStaticMeshComponent* Gate = CreateDefaultSubobject<UStaticMeshComponent>(GateSocket);
		// 생성된 컴포넌트에 메시 애셋 설정
		if (GateMeshRef.Succeeded())
		{
			Gate->SetStaticMesh(GateMeshRef.Object);
		}
		// 컴포넌트 계층 설정 (소켓 포함 지정)
		Gate->SetupAttachment(Stage, GateSocket);
		// 위치 / 회전 조정
		Gate->SetRelativeLocationAndRotation(
			FVector(0.0f, -83.0f, 0.0f), FRotator(0.0f, -90.0f, 0.0f)
		);
		// Map에 생성한 컴포넌트 추가
		Gates.Add(GateSocket, Gate);
		
		// 문 통과를 확인하기 위한 박스 컴포넌트
		// 예 : +XGate + Trigger = +XGateTrigger
		FName TriggerName = *GateSocket.ToString().Append("Trigger");
		UBoxComponent* GateTrigger = CreateDefaultSubobject<UBoxComponent>(TriggerName);
		// 박스 컴포넌트의 충돌 영역 설정
		GateTrigger->SetBoxExtent(FVector(100.0f, 100.0f, 300.0f));
		// 생성된 박스 컴포넌트의 계층 및 소켓 설정
		GateTrigger->SetupAttachment(Stage, GateSocket);
		// 상대 위치 조정
		GateTrigger->SetRelativeLocation(FVector(70.0f, 0.0f, 250.0f));
		// 콜리전 프로필 설정
		GateTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
		// 충돌 이벤트에 함수 등록 (델리게이트)
		GateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnGateTriggerBeginOverlap);
		
		// 트리거 구분을 위해 태그 추가
		GateTrigger->ComponentTags.Add(GateSocket);
		
		// 생성된 트리거 컴포넌트를 배열에 추가
		GateTriggers.Add(GateTrigger);
	}
}

void AABStageGimmick::OnStageTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
}

void AABStageGimmick::OnGateTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("Gate tag : %s"), *OverlappedComponent->ComponentTags[0].ToString());
}
