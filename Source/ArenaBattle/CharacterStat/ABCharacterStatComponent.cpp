// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/ABCharacterStatComponent.h"
#include "GameData/ABGameSingleton.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	//MaxHp = 200.0f;
	//CurrentHp = MaxHp;
	// 현재 레벨 설정
	CurrentLevel = 1;
	
	// 공격 반경 설정
	AttackRadius = 50.0f;
	
	bWantsInitializeComponent = true;
}

void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	// 시작할 때 현재 레벨에 맞는 스탯 데이터 설정
	SetLevelStat(CurrentLevel);
	SetHp(BaseStat.MaxHp);

	
}


// // Called when the game starts
// void UABCharacterStatComponent::BeginPlay()
// {
// 	Super::BeginPlay();
//
// 	// 게임이 시작되면 최대 체력에서 시작하도록 설정
// 	//SetHp(MaxHp);
// 	
// }

void UABCharacterStatComponent::SetHp(float NewHp)
{
	// 현재 체력 값 갱신
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, BaseStat.MaxHp);
	
	// 체력 변경 이벤트 발행
	OnHpChanged.Broadcast(CurrentHp);
}

void UABCharacterStatComponent::SetLevelStat(int32 InNewLevel)
{
	// 게임 싱글톤의 값을 사용해 현재 레벨 고정
	// 새로 설정할 레벨의 값이 최대 레벨을 벗어나지 않도록
	CurrentLevel = FMath::Clamp(InNewLevel, 1, UABGameSingleton::Get().CharacterMaxLevel);
	
	// 스탯 데이터 설정
	//BaseStat = UABGameSingleton::Get().GetCharacterStat(CurrentLevel);
	SetBaseStat(UABGameSingleton::Get().GetCharacterStat(CurrentLevel));
	
	// 확인
	ensureAlways(BaseStat.MaxHp > 0.0f);
}

float UABCharacterStatComponent::ApplyDamage(float InDamage)
{
	// 대미지 처리
	// 변경 여부를 확인하기 위해 대미지 처리 전 값 저장
	const float PrevHp = CurrentHp;
	// 대미지 값 가져오기
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0.0f, GetTotalStat().MaxHp);
	
	// 대미지 처리
	SetHp(PrevHp - ActualDamage);
	//CurrentHp = FMath::Clamp<float>(PrevHp - ActualDamage, 0.0f, MaxHp);
	
	// 죽었는지 여부 (체력이 0인지) 확인
	// 오차 범위
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		// 체력을 모두 소진했다(죽음)는 이벤트 발행
		OnHpZero.Broadcast();		
	}
	
	return ActualDamage;
}
