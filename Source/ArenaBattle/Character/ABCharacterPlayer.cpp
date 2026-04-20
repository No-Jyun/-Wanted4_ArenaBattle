// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterPlayer.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"


AABCharacterPlayer::AABCharacterPlayer()
{
	// 기본 설정
	// 컨트롤러의 회전 값을 받아서 설정하는 옵션 비활성화
	bUseControllerRotationPitch = false;	// Y 축 회전
	bUseControllerRotationYaw = false;		// Z 축 회전
	bUseControllerRotationRoll = false;		// X 축 회전

	// 무브먼트 설정
	// 캐릭터가 이동하는 방향에 맞게 회전을 해주는 옵션
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 800.0f;

	// 메시 컴포넌트 설정
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f)
	);

	// 메시 애셋 지정 (검색 필요함)
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMesh(
		TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple")
	);

	// 로드 성공했으면 설정
	if (CharacterMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(CharacterMesh.Object);
	}

	// 애님 블루프린트 클래스 정보 지정
	static ConstructorHelpers::FClassFinder<UAnimInstance> CharacterAnim(
		TEXT("/Game/Characters/Mannequins/Anims/Unarmed/ABP_Unarmed.ABP_Unarmed_C")
	);

	if (CharacterAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(CharacterAnim.Class);
	}

	// 컴포넌트 생성
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// 계층 설정 (루트 컴포넌트 아래로)
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 600.0f;
	// 컨트롤러의 회전 값을 사용할 지 여부
	SpringArm->bUsePawnControlRotation = true;

	// 카메라 컴포넌트
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// 입력 관련 애셋 로드 및 설정
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DefaultMappingContextRef(
		TEXT("/Game/ArenaBattle/Input/IMC_Default.IMC_Default")
	);

	if (DefaultMappingContextRef.Succeeded())
	{
		DefaultMappingContext = DefaultMappingContextRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionRef(
		TEXT("/Game/ArenaBattle/Input/Actions/IA_Move.IA_Move")
	);

	if (MoveActionRef.Succeeded())
	{
		MoveAction = MoveActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionRef(
		TEXT("/Game/ArenaBattle/Input/Actions/IA_Look.IA_Look")
	);

	if (LookActionRef.Succeeded())
	{
		LookAction = LookActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionRef(
		TEXT("/Game/ArenaBattle/Input/Actions/IA_Jump.IA_Jump")
	);

	if (JumpActionRef.Succeeded())
	{
		JumpAction = JumpActionRef.Object;
	}

	// 사용할 입력 매핑 컨텍스트 지정
	//GetController()->
	// Get Controller는 여기서 사용하면 안됨 (세팅 아직 안됨)
}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	// 사용할 입력 매핑 컨텍스트 지정
	// 다운 캐스팅
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		// 향상된 입력 시스템의 서브 시스템 가져오기
		UEnhancedInputLocalPlayerSubsystem* InputSystem = 
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		// 향상된 입력 서브 시스템 얻어온 후에 사용할 매핑 컨텍스트 설정
		if (InputSystem)
		{
			InputSystem->AddMappingContext(DefaultMappingContext, 0);
		}

	}
}

void AABCharacterPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// 바인딩 - 향상된 입력 시스템 컴포넌트를 활용해서 설정
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		// 입력 바인딩 -> 이벤트와 실행 함수를 연결하는 과정
		EnhancedInputComponent->BindAction(
			MoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Move
			);
		
		EnhancedInputComponent->BindAction(
			LookAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Look
			);
		
		EnhancedInputComponent->BindAction(
			JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump
			);
		
		EnhancedInputComponent->BindAction(
			JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping
			);
	}
}

void AABCharacterPlayer::Move(const FInputActionValue& Value)
{
	// 입력 값 읽어오기 (입력에 지정된 타입으로 변환)
	FVector2D Movement = Value.Get<FVector2D>();
	
	// 이동할 방향 만들기
	// 카메라가 바라보는 방향 (컨트롤러의 방향) 을 기준으로 방향 만들기
	FRotator Rotation = GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
	
	// 앞방향
	FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
	// 오른쪽 방향
	FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	// 무브먼트 컴포넌트에 입력 전달하기
	AddMovementInput(ForwardVector, Movement.Y);
	AddMovementInput(RightVector, Movement.X);
}

void AABCharacterPlayer::Look(const FInputActionValue& Value)
{
	// 입력 값 가져오기
	FVector2d RotationValue = Value.Get<FVector2D>();
	
	// 회전 처리 (카메라 회전)
	// 컨트롤러를 회전 시키면 스프링 암 컴포넌트가 회전함
	AddControllerYawInput(RotationValue.X);
	AddControllerPitchInput(-RotationValue.Y);	
	
	
	
	
	
	
}
