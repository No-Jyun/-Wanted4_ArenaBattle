// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"

AABCharacterPlayer::AABCharacterPlayer()
{
	// 기본 설정.
	// 컨트롤러의 회전 값을 받아서 설정하는 옵션 비활성화.
	bUseControllerRotationPitch = false;	// Y축 회전.
	bUseControllerRotationYaw = false;		// Z축 회전.
	bUseControllerRotationRoll = false;		// X축 회전.

	// 무브먼트 설정.
	// 캐릭터가 이동하는 방향에 맞게 회전을 해주는 옵션.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 800.0f;

	// 메시 컴포넌트 설정.
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, -88.0f),
		FRotator(0.0f, -90.0f, 0.0f)
	);

	// 메시 애셋 지정 (검색 필요함).
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMesh(
		TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple")
	);

	// 로드 성공했으면 설정.
	if (CharacterMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(CharacterMesh.Object);
	}

	// 애님 블루프린트 클래스 정보 지정.
	static ConstructorHelpers::FClassFinder<UAnimInstance> CharacterAnim(
		TEXT("/Game/Characters/Mannequins/Anims/Unarmed/ABP_Unarmed.ABP_Unarmed_C")
	);

	if (CharacterAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(CharacterAnim.Class);
	}

	// 컴포넌트 생성.
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(
		TEXT("SpringArm"));
	// 계층 설정 (루트 컴포넌트 아래로).
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 600.0f;
	// 컨트롤러의 회전 값을 사용할 지 여부.
	SpringArm->bUsePawnControlRotation = true;

	// 카메라 컴포넌트.
	Camera = CreateDefaultSubobject<UCameraComponent>(
		TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void AABCharacterPlayer::BeginPlay()
{
}
