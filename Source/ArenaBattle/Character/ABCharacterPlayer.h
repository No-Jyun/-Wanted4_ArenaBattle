// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ABCharacterBase.h"
#include "InputActionValue.h"
#include "ABCharacterPlayer.generated.h"

// 전방선언
class UInputAction;

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABCharacterPlayer : public AABCharacterBase
{
	GENERATED_BODY()
	
public:
	AABCharacterPlayer();

protected:
	virtual void BeginPlay() override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 설정된 컨트롤에 따라 입력 매핑 컨텍스트 및 관련 설정 처리
	void SetCharacterControl(ECharacterControlType NewCharacterControlType);
	
	// 컨트롤 데이터 설정
	virtual void SetCharacterControlData(const class UABCharacterControlData* InCharacterControlData) override;
	
protected:
	// 캐릭터 컨트롤 변경 입력 처리 함수
	void ChangeCharacterControl();
	
	// 이동 함수
	void ShoulderMove(const FInputActionValue& Value);
	
	void QuaterMove(const FInputActionValue& Value);
	
	// 회전 함수
	void ShoulderLook(const FInputActionValue& Value);
	
	void Attack();
	
protected:
	// 컴포넌트 구성
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<class UCameraComponent> Camera;
	
	// 입력 관련
protected:
	// UPROPERTY(VisibleAnywhere, Category = "Input", BlueprintReadOnly)
	// TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(VisibleAnywhere, Category = "Input", BlueprintReadOnly)
	TObjectPtr<UInputAction> ShoulderMoveAction;

	UPROPERTY(VisibleAnywhere, Category = "Input", BlueprintReadOnly)
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(VisibleAnywhere, Category = "Input", BlueprintReadOnly)
	TObjectPtr<UInputAction> ShoulderLookAction;

	UPROPERTY(VisibleAnywhere, Category = "Input", BlueprintReadOnly)
	TObjectPtr<UInputAction> QuaterMoveAction;

	UPROPERTY(VisibleAnywhere, Category = "Input", BlueprintReadOnly)
	TObjectPtr<UInputAction> ChangeControlAction;
	
	UPROPERTY(VisibleAnywhere, Category = "Input", BlueprintReadOnly)
	TObjectPtr<UInputAction> AttackAction;
	
	// 현재 사용중인 캐릭터 컨트롤 타입
	UPROPERTY(VisibleAnywhere, Category = "CharacterControl")
	ECharacterControlType CurrentCharacterControlType;
};
