// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/ABAnimationAttackInterface.h"
#include "Interface/ABCharacterWidgetInterface.h"
#include "ABCharacterBase.generated.h"

// 캐릭터 컨트롤 타입 (입력 방식을 설정하기 위함).
UENUM()
enum class ECharacterControlType : uint8
{
	Shoulder,
	Quarter
};

// 다중 상속 : 여러 부모를 상속하는 형태
// -> 쓰지 마시라
// -> 따라서 조건부로 잘 사용해야 함
// -> "클래스 상속" 은 하나만 허용
// -> "나머지 다중 상속은 모두 인터페이스로 (순수 가상 함수를 가지는 클래스) 만"
// -> 순수 가상 함수를 가진 클래스는 그 자체로는 인스턴스를 생성할 수 없음
// -> 로우 레벨 기준에서 왜 안될까??? 링커 - 선언은 있지만 정의가 없어서 오류 날듯
UCLASS()
class ARENABATTLE_API AABCharacterBase : public ACharacter, public IABAnimationAttackInterface, public IABCharacterWidgetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacterBase();
	
	// 공격 판정 함수
	virtual void AttackHitCheck() override;

	// 대미지 처리 함수
	virtual float TakeDamage(
		float DamageAmount, 
		struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, 
		AActor* DamageCauser) override;
	
	// Inherited via IABCharacterWidgetInterface
	virtual void SetupCharacterWidget(class UABUserWidget* InUserWidget) override;
	
	// 모든 컴포넌트
	virtual void PostInitializeComponents() override;
	
protected:
	// 죽음 설정 함수
	virtual void SetDead();
	
	// 죽는 애니메이션 재생 함수
	void PlayDeadAnimation();
	
protected:

	// 컨트롤 데이터를 설정하는 함수.
	virtual void SetCharacterContolData(
		const class UABCharacterControlData* InCharacterControlData
	);

	// 콤보 명령 처리 함수.
	// 플레이어 입력이 들어왔을 때 콤보 액션을 시작하거나 예약.
	void ProcessComboCommand();

	// 콤보 액션이 시작될 때 호출되는 함수.
	void ComboActionBegin();

	// 몽타주 재생 종료 시 호출될 함수 (애니메이션 몽타주 델리게이트 연결용).
	void ComboActionEnd(UAnimMontage* TargetMontage, bool bInterrupted);

	// 콤보 체크 타이머 설정 함수.
	void SetComboCheckTimer();

	// 콤보 체크 처리 함수.
	// 타이머를 종료하고, 유효 시간 내에 다음 입력이 들어왔는지 확인하여 처리.
	void ComboCheck();

protected:
	// 컨트롤 타입에 따른 컨트롤 데이터를 관리하는 맵.
	UPROPERTY(EditAnywhere, Category = CharacterControl)
	TMap<ECharacterControlType, class UABCharacterControlData*> CharacterControlManager;

protected:
	// 콤보 공격 몽타주.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	TObjectPtr<class UAnimMontage> ComboAttackMontage;

	// 콤보 액션 처리를 위한 데이터.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	TObjectPtr<class UABComboActionData> ComboActionData;

	// 현재 콤보 단계 (0은 공격 안함. 1/2/3/4 등 단계 지정).
	UPROPERTY(EditAnywhere, Category = Attack)
	uint32 CurrentCombo = 0;

	// 콤보 타이머 처리를 위한 핸들.
	FTimerHandle ComboTimerHandle;

	// 다음 콤보 명령이 예약되었는지 확인하는 플래그.
	UPROPERTY(VisibleAnywhere, Category = Attack)
	bool bHasNextComboCommand = false;
	
	// Dead Section
protected:
	// 죽음 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dead")
	TObjectPtr<class UAnimMontage> DeadMontage;

	// 죽은 후 대기할 시간 값 (단위 : 초)
	float DeadEventDelayTime = 5.0f;
	
protected:
	// 스탯 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	TObjectPtr<class UABCharacterStatComponent> Stat;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
	TObjectPtr<class UABWidgetComponent> HpBar;
};
