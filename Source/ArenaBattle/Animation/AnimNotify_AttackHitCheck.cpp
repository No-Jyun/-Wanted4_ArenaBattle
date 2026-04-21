// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_AttackHitCheck.h"
#include  "Interface/ABAnimationAttackInterface.h"

void UAnimNotify_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	// 애님 노티파이가 배치된 애니메이션을 사용(소유)하는 폰에 접근해서
	// 공격 판정하도록 함수 실행
	// 인터페이스를 활용 -> 다른 객체에 접근해야할 때 알고 싶은 정보만 노출
	if (MeshComp)
	{
		// 메시 컴포넌트를 소유하는 액터를 원하는 인터페이스 타입으로 변환
		// 인터페이스 장점 - 디커플링 패턴
		IABAnimationAttackInterface* AttackPawn = Cast<IABAnimationAttackInterface>(MeshComp->GetOwner());
		
		if (AttackPawn)
		{
			AttackPawn->AttackHitCheck();
		}
	}	
}
