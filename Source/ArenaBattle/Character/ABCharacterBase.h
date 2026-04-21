// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ABCharacterBase.generated.h"

// 열거형 (입력 컨트롤을 관리하기 위함)
UENUM()
enum class ECharacterControlType : uint8
{
	Shoulder,
	Quater
};

UCLASS()
class ARENABATTLE_API AABCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacterBase();

protected:
	// 컨트롤 데이터 설정
	virtual void SetCharacterControlData(
		const class UABCharacterControlData* InCharacterControlData
	);
	
	// 몽타주 재생 종료 시 호출할 함수 (델리게이트와 연동)
	void ComboActionEnd(UAnimMontage* TargetMontage, bool bInterrupted);
	
protected:
	// 컨트롤 타입 별 데이터 관리를 위한 맵
	UPROPERTY(EditAnywhere, Category = "CharacterControl")
	TMap<ECharacterControlType, class UABCharacterControlData*> CharacterControlManager;
	
protected:
	// 콤보 공격 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TObjectPtr<class UAnimMontage> ComboAttackMontage;
};
