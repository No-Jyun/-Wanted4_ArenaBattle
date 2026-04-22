// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ABCharacterBase.h"
#include "ABCharacterControlData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ABComboActionData.h"
#include "Components/CapsuleComponent.h"
#include "Physics/ABCollision.h"
#include "Engine/DamageEvents.h"

#include "CharacterStat/ABCharacterStatComponent.h"
#include "UI/ABHpBarWidget.h"
#include "UI/ABWidgetComponent.h"

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

	// 캐릭터 컨트롤 데이터 로드.
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

	// 죽음 몽타주 애셋 로드
	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(
		TEXT("/Game/ArenaBattle/Animation/AM_Dead.AM_Dead")
	);
	if (DeadMontageRef.Succeeded())
	{
		DeadMontage = DeadMontageRef.Object;
	}

	// 컴포넌트 생성
	Stat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("Stat"));
	HpBar = CreateDefaultSubobject<UABWidgetComponent>(TEXT("Widget"));

	// 참고 : ActorComponent 는 별도의 트랜스폼 정보가 없기 때문에 생성만 해주면 됨
	// SceneComponent는 별도의 트랜스폼 정보가 있지 때문에 다른 계층에 소속되도록 설명해줘야 함 (SetAttachment)

	// 계층 및 위치 설정
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));

	// 사용할 위젯 설정
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(
		TEXT("/Game/ArenaBattle/UI/WBP_HpBar.WBP_HpBar_C")
	);
	if (HpBarWidgetRef.Succeeded())
	{
		// 위젯 컴포넌트에서 사용할 위젯 클래스 지정
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		// 위젯이 그려질 공간 설정 (3D / 2D)
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		// 위젯이 그려질 크기
		HpBar->SetDrawSize(FVector2D(150.0f, 15.0f));
		// 콜리전 끄기
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AABCharacterBase::AttackHitCheck()
{
	// 공격 범위
	const float AttackRange = 300.0f;

	// 트레이스에 사용할 구체의 반지름
	const float AttackRadius = 50.0f;

	// 콜리전에 사용할 콜리전 파라미터 설정
	// 나를 제외해달라고 설정하기 위해
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
	// - 단점 : 짐벌락 (Gimbal Lock) (두 축이 한 축처럼 붙어서 소멸하는 문제) / 계산량 많음
	// 쿼터니언 (4원수) : x/y/z/w -> 4개의 수로 3차원 회전을 표기
	// - 장점 : 짐벌락 없음. 정확함. 계산량 적음.
	// - 단점 : 직관적으로 알 수가 없음
	DrawDebugCapsule(
		GetWorld(),
		CapsuleOrigin,
		CapsuleHalfHeight,
		AttackRadius,
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
		DrawColor, false, 5.0f);
#endif
}

float AABCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                   class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 죽음 설정
	//SetDead();
	Stat->ApplyDamage(DamageAmount);

	return DamageAmount;
}

void AABCharacterBase::SetupCharacterWidget(class UABUserWidget* InUserWidget)
{
	// 의존성 주입 (Dependency Injection)
	// 캐릭터 입장 : 누군가 이 함수를 호출하면서 UABUserWidget 정보를 전달해줌
	UABHpBarWidget* HpBarWidget = Cast<UABHpBarWidget>(InUserWidget);
	if (HpBarWidget)
	{
		// 체력 관련 값 설정
		HpBarWidget->SetMaxHp(Stat->GetMaxHp());
		HpBarWidget->UpdateHpBar(Stat->GetCurrentHp());
		
		// 델리게이트 등록
		Stat->OnHpChanged.AddUObject(HpBarWidget, &UABHpBarWidget::UpdateHpBar);
	}
}

void AABCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// 스탯 컴포넌트의 죽음 델리게이트에 함수 연결
	Stat->OnHpZero.AddUObject(this, &AABCharacterBase::SetDead);
}

void AABCharacterBase::SetDead()
{
	// 죽었을 때 필요한 정리 작업

	// 무브먼트 끄기
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// 죽는 모션 재생 (몽타주 재생 요청)
	PlayDeadAnimation();

	// 콜리전 끄기
	SetActorEnableCollision(false);
	
	// 죽으면 Hp 바 사라지게 설정
	HpBar->SetHiddenInGame(true);
}

void AABCharacterBase::PlayDeadAnimation()
{
	// 몽타주 재생을 위해 애님 인스턴스 가져오기
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// 재생 중일 수 있는 몽타주 모두 종료
		AnimInstance->StopAllMontages(0.0f);

		// 죽음 몽타주 재생
		AnimInstance->Montage_Play(DeadMontage);
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
	// 처음 콤보 공격일 때 처리.
	if (CurrentCombo == 0)
	{
		ComboActionBegin();
		return;
	}

	// 공격이 이미 진행 중일 때 처리.
	// 타이머 핸들의 유효 여부에 따른 입력 처리.
	if (ComboTimerHandle.IsValid())
	{
		// 입력이 예약된 경우 상태.
		bHasNextComboCommand = true;
	}
	else
	{
		// 입력이 예약되지 않은 경우 상태.
		bHasNextComboCommand = false;
	}
}

void AABCharacterBase::ComboActionBegin()
{
	// 현재 콤보 단계를 1단계로 설정.
	CurrentCombo = 1;

	// 애님 인스턴스 가져오기.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// 몽타주 재생 속도.
		const float AttackSpeedRate = 1.0f;

		// 몽타주 재생.
		AnimInstance->Montage_Play(ComboAttackMontage, AttackSpeedRate);

		// 몽타주 재생 종료 시 호출되는 이벤트에 등록.
		FOnMontageEnded OnMontageEnded;
		OnMontageEnded.BindUObject(this, &AABCharacterBase::ComboActionEnd);

		// 몽타주 종료 델리게이트 세팅.
		AnimInstance->Montage_SetEndDelegate(OnMontageEnded, ComboAttackMontage);

		// 몽타주 재생 중 이동 불가능하도록 설정.
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

		// 콤보 타이머를 확인하기 위한 타이머 설정.
		// 기존에 실행된 타이머를 무효화.
		ComboTimerHandle.Invalidate();

		// 타이머 설정.
		SetComboCheckTimer();
	}
}

void AABCharacterBase::ComboActionEnd(
	UAnimMontage* TargetMontage, bool bInterrupted)
{
	// 값 확인 (어설트).
	ensureAlways(CurrentCombo > 0);

	// 콤보 단계 초기화.
	CurrentCombo = 0;

	// 몽타주 재생이 종료되면 캐릭터 이동을 다시 가능 상태로 변경.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AABCharacterBase::SetComboCheckTimer()
{
	// 현재 진행중인 콤보 단계의 인덱스 계산.
	const int32 ComboIndex = CurrentCombo - 1;

	// 확인.
	ensureAlways(
		ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIndex)
	);

	// 애니메이션 재생 속도를 고려(적용).
	const float AttackSpeedRate = 1.0f;

	// 타이머 설정에 사용할 시간 값.
	// 콤보 액션 데이터에서 프레임 수로 저장되어 있음.
	// 프레임(애니메이션) -> 초단위로 변환이 필요함.
	// 대략 17/30 -> 0.56666초.
	float ComboEffectTime = (ComboActionData->EffectiveFrameCount[ComboIndex]
		/ ComboActionData->FrameRate) / AttackSpeedRate;

	// 타이머 설정.
	if (ComboEffectTime > 0)
	{
		// 시간이 0보다 크면.
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
	// 타이머 체크가 끝난 타이머 핸들 초기화.
	ComboTimerHandle.Invalidate();

	// 콤보 타이머 내에 다음 입력이 들어왔는지 확인.
	if (bHasNextComboCommand)
	{
		// 몽타주 섹션 변경.
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			// 다음 콤보 단계 설정.
			CurrentCombo = FMath::Clamp(
				CurrentCombo + 1,
				1,
				ComboActionData->MaxComboCount
			);

			// 플레이할 섹션 이름 생성.
			FName NextSection = *FString::Printf(
				TEXT("%s%d"),
				*ComboActionData->MontageSectionNamePrefix,
				CurrentCombo
			);

			// 몽타주 해당 섹션으로 점프.
			AnimInstance->Montage_JumpToSection(
				NextSection,
				ComboAttackMontage
			);

			// 타이머 재설정.
			SetComboCheckTimer();

			// 콤보 처리를 마친 후 초기화.
			bHasNextComboCommand = false;
		}
	}
}
