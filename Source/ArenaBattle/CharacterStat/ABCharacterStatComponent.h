// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameData/ABCharacterStat.h"
#include "ABCharacterStatComponent.generated.h"

// 델리게이트 선언
DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChanged, float /*CurrentHp*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Setter
	void SetHp(float NewHp);
	
public:
	// Getter
	//FORCEINLINE float GetMaxHp() const {return MaxHp;}
	void SetLevelStat(int32 InNewLevel);
	FORCEINLINE int32 GetCurrentLevel() const {return CurrentLevel;}
	FORCEINLINE void SetModifierStat(const FABCharacterStat& InModifierStat)
	{
		ModifierStat = InModifierStat;
	}
	FORCEINLINE FABCharacterStat GetTotalStat() const
	{
		// 최종 스탯 = 기본 스탯 + 부가 스탯
		return BaseStat + ModifierStat;
	}
	FORCEINLINE float GetCurrentHp() const {return CurrentHp;}
	
	// 대미지 적용 함수
	float ApplyDamage(float InDamage);
	
public:
	// 체력을 모두 소진했을 때 발행할 델리게이트
	FOnHpZeroDelegate OnHpZero;
	
	// Hp가 변동될 때마다 발행할 델리게이트
	FOnHpChanged OnHpChanged;
	
protected:
	// 체력 정보
	
	// 최대 체력
	// VisibleInstanceOnly : 클래스 정보에서는 안보이고, 실제 생성된 객체 정보에서만 확인 가능
	//UPROPERTY(VisibleInstanceOnly, Category = "Stat")
	//float MaxHp;
	
	// 현재 체력
	// Transient (임시) : 현재 체력의 경우에는 게임을 진행할 때마다 자주 변경되기 때문에 굳이 디스크에 저장 필요하지 않음
	UPROPERTY(Transient, VisibleInstanceOnly, Category = "Stat")
	float CurrentHp;
	
	// 현재 레벨
	UPROPERTY(Transient, VisibleInstanceOnly, Category = "Stat")
	int32 CurrentLevel;
	
	// 캐릭터 기본 스탯 데이터
	UPROPERTY(Transient, VisibleInstanceOnly, Category = "Stat")
	FABCharacterStat BaseStat;
	
	// 아이템을 통해 부가로 얻을 추가 스탯 데이터
	UPROPERTY(Transient, VisibleInstanceOnly, Category = "Stat")
	FABCharacterStat ModifierStat;
}; 
