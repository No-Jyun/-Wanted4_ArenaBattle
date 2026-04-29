// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/ABCharacterStat.h"
#include "ABHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 생성자
	UABHUDWidget(const FObjectInitializer& ObjectInitializer);

public:
	// 캐릭터에서 호출할 함수
	// 래퍼 함수로 이 위젯이 관리하는 하위 위젯에 메시지 전달
	void UpdateStat(
		const FABCharacterStat& BaseStat,
		const FABCharacterStat& ModifierStat
	);

	void UpdateHpBar(float NewCurrentHp);

protected:
	// AddToViewport 함수 호출 이후에 호출되는 함수
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	TObjectPtr<class UABHpBarWidget> HpBar;

	UPROPERTY()
	TObjectPtr<class UABCharacterStatWidget> CharacterStat;
};
