// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ABCharacterBase.h"
#include "ABCharacterControlData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ABComboActionData.h"

// Sets default values
AABCharacterBase::AABCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 맵 설정.
	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> ShoulderDataRef(
		TEXT("/Game/ArenaBattle/CharacterControl/ABC_Shoulder.ABC_Shoulder")
	);
	if (ShoulderDataRef.Succeeded())
	{
		CharacterControlManager.Add(
			ECharacterControlType::Shoulder,
			ShoulderDataRef.Object
		);
	}

	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> QuarterDataRef(
		TEXT("/Game/ArenaBattle/CharacterControl/ABC_Quater.ABC_Quater")
	);
	if (QuarterDataRef.Succeeded())
	{
		CharacterControlManager.Add(
			ECharacterControlType::Quarter,
			QuarterDataRef.Object
		);
	}
}

void AABCharacterBase::SetCharacterContolData(
	const UABCharacterControlData* InCharacterControlData)
{
	// Pawn.
	bUseControllerRotationYaw
		= InCharacterControlData->bUseControllerRotationYaw;

	// CharacterMovement.
	GetCharacterMovement()->bUseControllerDesiredRotation
		= InCharacterControlData->bUseControllerDesiredRotation;

	GetCharacterMovement()->bOrientRotationToMovement
		= InCharacterControlData->bOrientRotationToMovement;

	GetCharacterMovement()->RotationRate
		= InCharacterControlData->RotationRate;
}

void AABCharacterBase::ProcessComboCommand()
{
	// 처음 공격 시작할 때 처리.
	if (CurrentCombo == 0)
	{
		ComboActionBegin();
		return;
	}

	// 공격이 이미 진행 중일 때 처리.
	// 타이머 핸들의 유효 여부에 따라 분기 처리.
	if (ComboTimerHandle.IsValid())
	{
		// 입력이 제대로 들어온 경우.
		bHasNextComboCommand = true;
	}
	else
	{
		// 입력이 제대로 들어오지 않은 경우.
		bHasNextComboCommand = false;
	}
}

void AABCharacterBase::ComboActionBegin()
{
	// 현재 콤보 단계를 1단계로 설정.
	CurrentCombo = 1;

	// 몽타주 재생.
	// 애님 인스턴스 가져오기.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// 몽타주 재생 속도.
		const float AttackSpeedRate = 1.0f;

		// 몽타주 재생.
		AnimInstance->Montage_Play(ComboAttackMontage, AttackSpeedRate);

		// 몽타주 종료 이벤트에 등록할 델리게이트 설정.
		FOnMontageEnded OnMontageEnded;
		OnMontageEnded.BindUObject(this, &AABCharacterBase::ComboActionEnd);

		// 몽타주 재생 종료 시 발행되는 이벤트에 등록.
		AnimInstance->Montage_SetEndDelegate(OnMontageEnded, ComboAttackMontage);

		// 몽타주 재생 시 이동 안하도록 설정.
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

		// 콤보 타이밍을 확인하기 위한 타이머 설정.
		// 기존에 설정된 타이머를 무효화.
		ComboTimerHandle.Invalidate();

		// 타이머 설정.
		SetComboCheckTimer();
	}
}

void AABCharacterBase::ComboActionEnd(
	UAnimMontage* TargetMontage, bool bInterrupted)
{
	// 값 확인 (어서트).
	ensureAlways(CurrentCombo > 0);

	// 콤보 단계 초기화.
	CurrentCombo = 0;

	// 몽타주 재생이 종료되면 캐릭터 이동을 다시 원상 복구.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AABCharacterBase::SetComboCheckTimer()
{
	// 현재 재생중인 콤보 단계의 인덱스 계산.
	const int32 ComboIndex = CurrentCombo - 1;

	// 확인.
	ensureAlways(
		ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIndex)
	);

	// 애니메이션 재생 속도도 고려(배속).
	const float AttackSpeedRate = 1.0f;

	// 타이머 설정에 사용할 시간 값.
	// 콤보 액션 데이터에는 프레임 값이 설정되어 있음.
	// 프레임(애니메이션) -> 초단위로 변환이 필요함.
	// 대략 17/30 -> 0.56666초.
	float ComboEffectTime = (ComboActionData->EffectiveFrameCount[ComboIndex]
		/ ComboActionData->FrameRate) / AttackSpeedRate;

	// 타이머 설정.
	if (ComboEffectTime > 0)
	{
		// 시간은 월드가 관리.
		GetWorld()->GetTimerManager().SetTimer(
			ComboTimerHandle,
			this,
			&AABCharacterBase::ComboCheck,
			ComboEffectTime,
			false
		);
	}
}

void AABCharacterBase::ComboCheck()
{
	// 타이머 재사용을 위해 타이머 핸들 초기화.
	ComboTimerHandle.Invalidate();

	// 콤보 타이머 전에 공격 입력이 들어왔는지 확인.
	if (bHasNextComboCommand)
	{
		// 몽타주 섹션 점프.
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			// 다음 콤보 단계 설정.
			CurrentCombo = FMath::Clamp(
				CurrentCombo + 1,
				1,
				ComboActionData->MaxComboCount
			);

			// 점프할 섹션 이름 구성.
			FName NextSection = *FString::Printf(
				TEXT("%s%d"),
				*ComboActionData->MontageSectionNamePrefix,
				CurrentCombo
			);

			// 몽타주 섹션 점프.
			AnimInstance->Montage_JumpToSection(
				NextSection, 
				ComboAttackMontage
			);

			// 타이머 재설정.
			SetComboCheckTimer();

			// 콤보 처리에 사용한 값 초기화.
			bHasNextComboCommand = false;
		}
	}
}
