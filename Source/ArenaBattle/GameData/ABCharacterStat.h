#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ABCharacterStat.generated.h"

// 데이터 테이블을 임포트할 때 행 (Row) 데이터를 정의하는 구조체
// Row-Data = Record (레코드)
USTRUCT(BlueprintType)
struct FABCharacterStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	FABCharacterStat()
		: MaxHp(0.0f),
		  Attack(0.0f),
		  AttackRange(0.0f),
		  AttackSpeed(0.0f),
		  MovementSpeed(0.0f)
	{
	}

	// 두 구조체를 더하는 연산자 오버로딩
	FABCharacterStat operator+(const FABCharacterStat& Other)
	{
		// 포인터 연산을 활용한 덧셈 처리
		// 모든 속성의 크기가 같아야함
		const float* const ThisPtr = reinterpret_cast<const float* const>(this);
		
		const float* const OtherPtr = reinterpret_cast<const float* const>(&Other);
		
		// 결과를 저장할 구조체
		FABCharacterStat Result;
		
		// 결과 구조체 포인터
		float* ResultPtr = reinterpret_cast<float*>(&Result);
		
		// 루프로 처리를 위해 float 변수의 개수 확인
		const int32 StatNum = sizeof(FABCharacterStat) / sizeof(float);
		
		for (int32 Index = 0; Index < StatNum; ++Index)
		{
			ResultPtr[Index] = ThisPtr[Index] + OtherPtr[Index];
		}
		
		return Result;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MovementSpeed;
};
