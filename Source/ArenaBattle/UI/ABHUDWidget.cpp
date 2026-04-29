// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABHUDWidget.h"
#include "UI/ABCharacterStatWidget.h"
#include "UI/ABHpBarWidget.h"
#include "Interface/ABCharacterHUDInterface.h"

UABHUDWidget::UABHUDWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UABHUDWidget::UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	// HpBar의 최대 체력 스탯 업데이트
	FABCharacterStat TotalStat = BaseStat + ModifierStat;
	HpBar->SetMaxHp(TotalStat.MaxHp);
	
	// 캐릭터 스탯 UI에 메시지 전달
	CharacterStat->UpdateStat(BaseStat, ModifierStat);
}

void UABHUDWidget::UpdateHpBar(float NewCurrentHp)
{
	// HpBar 위젯에 메시지 전달
	HpBar->UpdateHpBar(NewCurrentHp);
}

void UABHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 위젯 블루프린트에 배치된 UI 위젯을 이름으로 검색
	HpBar = Cast<UABHpBarWidget>(GetWidgetFromName(TEXT("WidgetHpBar")));

	// 제대로 설정됐는지 확인
	ensureAlways(HpBar);

	CharacterStat = Cast<UABCharacterStatWidget>(GetWidgetFromName(TEXT("WidgetCharacterStat")));

	ensureAlways(CharacterStat);

	// 인터페이스를 통해 이 위젯을 사용하는 캐릭터에 초기화 요청
	//IABCharacterHUDInterface* HUDPawn = Cast<IABCharacterHUDInterface>(GetOwningPlayerPawn());
	IABCharacterHUDInterface* HUDPawn = GetOwningPlayerPawn<IABCharacterHUDInterface>();
	if (HUDPawn)
	{
		HUDPawn->SetupHUDWidget(this);
	}
}
