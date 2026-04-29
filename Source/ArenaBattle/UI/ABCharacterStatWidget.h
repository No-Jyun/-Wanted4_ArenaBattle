// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/ABCharacterStat.h"
#include "ABCharacterStatWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABCharacterStatWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	// BaseStat, ModifierStat 정보를 받아 UI를 업데이트하는 함수
	// Stat 컴포넌트에 이벤트 바인딩 (델리게이트) 해 호출되도록 설정
	void UpdateStat(
		const FABCharacterStat& BaseStat,
		const FABCharacterStat& ModifierStat
	);
	
private:
	// 스탯 위젯이 가진 텍스트 블록을 저장하는 맵 (BaseStat)
	UPROPERTY()
	TMap<FName, class UTextBlock*> BaseLookup;
	
	// 스탯 위젯이 가진 텍스트 블록을 저장하는 맵 (ModifierStat)
	UPROPERTY()
	TMap<FName, class UTextBlock*> ModifierLookup;
};
