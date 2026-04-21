// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ABCharacterBase.h"
#include "ABCharacterControlData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ABComboActionData.h"
#include "Components/CapsuleComponent.h"
#include "Physics/ABCollision.h"
#include "Engine/DamageEvents.h"

// Sets default values
AABCharacterBase::AABCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 콜리전 설정
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_ABCAPSULE);
	
	// 메시 컴포넌트 설정.
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, -88.0f),
		FRotator(0.0f, -90.0f, 0.0f)
	);

	// 메시 애셋 지정 (검색 필요함).
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMesh(
		TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Robo.SK_CharM_Robo")
	);

	// 로드 성공했으면 설정.
	if (CharacterMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(CharacterMesh.Object);
	}

	// 애님 블루프린트 클래스 정보 지정.
	static ConstructorHelpers::FClassFinder<UAnimInstance> CharacterAnim(
		TEXT("/Game/ArenaBattle/Animation/ABP_ABCharacter.ABP_ABCharacter_C")
	);

	if (CharacterAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(CharacterAnim.Class);
	}
	
	// 메시 콜리전 끄기
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	// �� ����.
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
	
	// 몽타주 및 액션 데이터 기본 값 설정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ComboAttackMontageRef(
		TEXT("/Game/ArenaBattle/Animation/AM_ComboAttack.AM_ComboAttack")
	);
	if (ComboAttackMontageRef.Succeeded())
	{
		ComboAttackMontage = ComboAttackMontageRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UABComboActionData> ComboActionDataRef(
		TEXT("/Game/ArenaBattle/ComboData/ABA_ComboAction.ABA_ComboAction")
	);
	if (ComboActionDataRef.Succeeded())
	{
		ComboActionData = ComboActionDataRef.Object;
	}
}

void AABCharacterBase::AttackHitCheck()
{
	// 공격 범위
	const float AttackRange = 300.0f;
	
	// 트레이스에 사용할 구체의 반지름
	const float AttackRadius = 50.0f;
	
	// 콜리전에 사용할 콜리전 파라미터 설정
	// 나를 제외해달라고 설정하지 위해
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);
	
	// 충돌 결과 정보를 반환 받을 변수
	FHitResult OutHitResult;
	
	// 트레이스 시작 위치
	// 액터 위치 + 캡슐의 반지름 만큼 앞으로 떨어뜨린 위치
	FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	
	// 트레이스 끝 위치
	// 시작 위치 + 공격 범위 만큼 앞으로 떨어뜨린 위치
	FVector End = Start + GetActorForwardVector() * AttackRange;
	
	// 충돌 판정 처리
	bool HitDetected = GetWorld()->SweepSingleByChannel(
		OutHitResult, Start, End, 
		FQuat::Identity, 
		CCHANNEL_ABACTION, 
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);
	
	// 충돌이 감지되었으면 대미지 전달
	if (HitDetected)
	{
		// 전달할 대미지 양
		const float AttackDamage = 30.0f;
		
		// 대미지 이벤트 변수
		FDamageEvent DamageEvent;
		
		// 충돌이 감지된 액터에 TakeDamage 함수 호출
		OutHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
	}
	
#if ENABLE_DRAW_DEBUG	
	// 캡슐의 중심 위치
	FVector CapsuleOrigin = Start + (End - Start) * 0.5f; 
	
	// 캡슐 높이의 반
	float CapsuleHalfHeight = AttackRange * 0.5f;
	
	// 표시할 색상
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
	
	// 충돌 디버깅 정보 시각화
	// 오일러 (x/y/z 축 정보로 회전을 표기하는 방식)
	// - 장점 : 직관적으로 이해 가능 -> 사람이 좋아함
	// - 단점 : 짐(김) 벌락 (Gimbal Lock) (두 축이 한 축처럼 붙어서 소멸하는 문제) / 계산량 많음
	// 쿼터니언 (4원수) : x/y/z/w -> 4개의 수로 3차원 회전을 표기
	// - 장점 : 짐벌락 없음. 정확함. 계산량 적음.
	// - 단점 : 알 수가 없음
	DrawDebugCapsule(
		GetWorld(),
		CapsuleOrigin,
		CapsuleHalfHeight,
		AttackRadius,
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
		DrawColor,false, 5.0f);
#endif
}

float AABCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	// 죽음 설정
	SetDead();
	
	return DamageAmount;
}

void AABCharacterBase::SetDead()
{
	
}

void AABCharacterBase::PlayDeadAnimation()
{
	
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
	// ó�� ���� ������ �� ó��.
	if (CurrentCombo == 0)
	{
		ComboActionBegin();
		return;
	}

	// ������ �̹� ���� ���� �� ó��.
	// Ÿ�̸� �ڵ��� ��ȿ ���ο� ���� �б� ó��.
	if (ComboTimerHandle.IsValid())
	{
		// �Է��� ����� ���� ���.
		bHasNextComboCommand = true;
	}
	else
	{
		// �Է��� ����� ������ ���� ���.
		bHasNextComboCommand = false;
	}
}

void AABCharacterBase::ComboActionBegin()
{
	// ���� �޺� �ܰ踦 1�ܰ�� ����.
	CurrentCombo = 1;

	// ��Ÿ�� ���.
	// �ִ� �ν��Ͻ� ��������.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// ��Ÿ�� ��� �ӵ�.
		const float AttackSpeedRate = 1.0f;

		// ��Ÿ�� ���.
		AnimInstance->Montage_Play(ComboAttackMontage, AttackSpeedRate);

		// ��Ÿ�� ���� �̺�Ʈ�� ����� ��������Ʈ ����.
		FOnMontageEnded OnMontageEnded;
		OnMontageEnded.BindUObject(this, &AABCharacterBase::ComboActionEnd);

		// ��Ÿ�� ��� ���� �� ����Ǵ� �̺�Ʈ�� ���.
		AnimInstance->Montage_SetEndDelegate(OnMontageEnded, ComboAttackMontage);

		// ��Ÿ�� ��� �� �̵� ���ϵ��� ����.
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

		// �޺� Ÿ�̹��� Ȯ���ϱ� ���� Ÿ�̸� ����.
		// ������ ������ Ÿ�̸Ӹ� ��ȿȭ.
		ComboTimerHandle.Invalidate();

		// Ÿ�̸� ����.
		SetComboCheckTimer();
	}
}

void AABCharacterBase::ComboActionEnd(
	UAnimMontage* TargetMontage, bool bInterrupted)
{
	// �� Ȯ�� (�Ʈ).
	ensureAlways(CurrentCombo > 0);

	// �޺� �ܰ� �ʱ�ȭ.
	CurrentCombo = 0;

	// ��Ÿ�� ����� ����Ǹ� ĳ���� �̵��� �ٽ� ���� ����.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AABCharacterBase::SetComboCheckTimer()
{
	// ���� ������� �޺� �ܰ��� �ε��� ���.
	const int32 ComboIndex = CurrentCombo - 1;

	// Ȯ��.
	ensureAlways(
		ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIndex)
	);

	// �ִϸ��̼� ��� �ӵ��� ����(���).
	const float AttackSpeedRate = 1.0f;

	// Ÿ�̸� ������ ����� �ð� ��.
	// �޺� �׼� �����Ϳ��� ������ ���� �����Ǿ� ����.
	// ������(�ִϸ��̼�) -> �ʴ����� ��ȯ�� �ʿ���.
	// �뷫 17/30 -> 0.56666��.
	float ComboEffectTime = (ComboActionData->EffectiveFrameCount[ComboIndex]
		/ ComboActionData->FrameRate) / AttackSpeedRate;

	// Ÿ�̸� ����.
	if (ComboEffectTime > 0)
	{
		// �ð��� ���尡 ����.
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
	// Ÿ�̸� ������ ���� Ÿ�̸� �ڵ� �ʱ�ȭ.
	ComboTimerHandle.Invalidate();

	// �޺� Ÿ�̸� ���� ���� �Է��� ���Դ��� Ȯ��.
	if (bHasNextComboCommand)
	{
		// ��Ÿ�� ���� ����.
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			// ���� �޺� �ܰ� ����.
			CurrentCombo = FMath::Clamp(
				CurrentCombo + 1,
				1,
				ComboActionData->MaxComboCount
			);

			// ������ ���� �̸� ����.
			FName NextSection = *FString::Printf(
				TEXT("%s%d"),
				*ComboActionData->MontageSectionNamePrefix,
				CurrentCombo
			);

			// ��Ÿ�� ���� ����.
			AnimInstance->Montage_JumpToSection(
				NextSection, 
				ComboAttackMontage
			);

			// Ÿ�̸� �缳��.
			SetComboCheckTimer();

			// �޺� ó���� ����� �� �ʱ�ȭ.
			bHasNextComboCommand = false;
		}
	}
}
