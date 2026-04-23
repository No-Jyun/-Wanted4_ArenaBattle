// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABStageGimmick.generated.h"

UCLASS()
class ARENABATTLE_API AABStageGimmick : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABStageGimmick();

	// Stage Section
protected:
	// 스테이지를 보여줄 메시 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Stage")
	TObjectPtr<class UStaticMeshComponent> Stage;
	
	// 박스 컴포넌트 (오버랩 이벤트 처리를 위한 컴포넌트)
	UPROPERTY(VisibleAnywhere, Category = "Stage")
	TObjectPtr<class UBoxComponent> StageTrigger;
	
	// StageTrigger에 등록할 오버랩 이벤트 처리 함수
	UFUNCTION()
	void OnStageTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult
	);

	// Gate Section (문 4개)
protected:
	// 4개의 문을 Map 으로 관리
	UPROPERTY(VisibleAnywhere, Category = "Gate")
	TMap<FName, TObjectPtr<class UStaticMeshComponent>> Gates;
	
	// 각 문을 담당하는 콜리전을 배열로 관리
	UPROPERTY(VisibleAnywhere, Category = "Gate")
	TArray<TObjectPtr<class UBoxComponent>> GateTriggers;
	
	// 각 문의 이벤트를 처리할 함수
	UFUNCTION()
	void OnGateTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult
	);
	
	
	
};
