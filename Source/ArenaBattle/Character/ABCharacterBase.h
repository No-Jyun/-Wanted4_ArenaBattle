// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include  "Interface/ABAnimationAttackInterface.h"
#include "ABCharacterBase.generated.h"

// ������ (�Է� ��Ʈ���� �����ϱ� ����).
UENUM()
enum class ECharacterControlType : uint8
{
	Shoulder,
	Quarter
};

UCLASS()
class ARENABATTLE_API AABCharacterBase : public ACharacter, public IABAnimationAttackInterface
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
	
protected:
	// 죽음 설정 함수
	virtual void SetDead();
	
	// 죽는 애니메이션 재생 함수
	void PlayDeadAnimation();
	
protected:

	// ��Ʈ�� ������ ����.
	virtual void SetCharacterContolData(
		const class UABCharacterControlData* InCharacterControlData
	);

	// �޺� ���� ó�� �Լ�.
	// ������ ó�� ������ ���� �޺� �׼��� ������ �� ����.
	void ProcessComboCommand();

	// �޺� ������ ���۵� �� ������ �Լ�.
	void ComboActionBegin();

	// ��Ÿ�� ��� ���� �� ȣ���� �Լ� (��������Ʈ�� ����).
	void ComboActionEnd(UAnimMontage* TargetMontage, bool bInterrupted);

	// �޺� Ÿ�̸� ���� �Լ�.
	void SetComboCheckTimer();

	// �޺� Ÿ�̹� ó�� �Լ�.
	// Ÿ�̸Ӹ� �����ϰ�, ������ �ð��� ������ ����.
	// �ð� ���� ���� �Է��� ����� ���Դ��� Ȯ��.
	void ComboCheck();

protected:
	// ��Ʈ�� Ÿ�� �� ������ ������ ���� ��.
	UPROPERTY(EditAnywhere, Category = CharacterControl)
	TMap<ECharacterControlType, class UABCharacterControlData*> CharacterControlManager;

protected:
	// �޺� ���� ��Ÿ��.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	TObjectPtr<class UAnimMontage> ComboAttackMontage;

	// �޺� �׼� ó�� ������.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	TObjectPtr<class UABComboActionData> ComboActionData;

	// ���� �޺� �ܰ� (0�� ���� �ȵ� ����. 1/2/3/4�� �ܰ� ����).
	UPROPERTY(EditAnywhere, Category = Attack)
	uint32 CurrentCombo = 0;

	// �޺� ������ ����� Ÿ�̸�.
	FTimerHandle ComboTimerHandle;

	// �޺� ������ ������ �� ����� �÷���.
	UPROPERTY(VisibleAnywhere, Category = Attack)
	bool bHasNextComboCommand = false;
};
