// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ABComboActionData.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABComboActionData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UABComboActionData();

	// 섹션 접두사(ComboAttack).
	UPROPERTY(EditAnywhere, Category = Name)
	FString MontageSectionNamePrefix;

	// 최대 콤보 수 (4개).
	UPROPERTY(EditAnywhere, Category = ComboData)
	uint32 MaxComboCount;

	// 프레임 재생 속도 (애니메이션 애셋에서 확인).
	UPROPERTY(EditAnywhere, Category = ComboData)
	float FrameRate;

	// 콤보 공격 마다의 콤보 판정에 사용할 프레임 값.
	// 입력이 이 프레임 이전에 들어왔는지 판정하는데 활용.
	UPROPERTY(EditAnywhere, Category = ComboData)
	TArray<float> EffectiveFrameCount;
};
