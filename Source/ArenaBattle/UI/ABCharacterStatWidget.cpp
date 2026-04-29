// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABCharacterStatWidget.h"

#include "Components/TextBlock.h"

void UABCharacterStatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 맵 구성을 위한 리플렉션 활용
	// FABCharacterStat 구조체 정보에서 변수 이름을 활용
	for (TFieldIterator<FNumericProperty> PropIt(
		     FABCharacterStat::StaticStruct());
	     PropIt != nullptr;
	     ++PropIt)
	{
		// 속성의 이름 값
		const FName PropKey(PropIt->GetName());

		// 속성의 이름 값을 활용해 텍스트 블록의 이름 값 만들기
		// Base 스탯에 대한 이름 값
		const FName TextBaseControlName =
			*FString::Printf(TEXT("Txt%sBase"), *PropIt->GetName());

		// Modifier 스탯에 대한 이름 값
		const FName TextModifierControlName =
			*FString::Printf(TEXT("Txt%sModifier"), *PropIt->GetName());

		// 위젯 블루프린트에서 위젯 컨트롤 읽어오기
		UTextBlock* BaseTextBlock = Cast<UTextBlock>(GetWidgetFromName(TextBaseControlName));
		if (BaseTextBlock)
		{
			// 맵에 추가
			BaseLookup.Add(PropKey, BaseTextBlock);
		}

		// 위젯 블루프린트에서 위젯 컨트롤 읽어오기
		UTextBlock* ModifierTextBlock = Cast<UTextBlock>(GetWidgetFromName(TextModifierControlName));
		if (ModifierTextBlock)
		{
			// 맵에 추가
			ModifierLookup.Add(PropKey, ModifierTextBlock);
		}
	}
}

void UABCharacterStatWidget::UpdateStat(
	const FABCharacterStat& BaseStat,
	const FABCharacterStat& ModifierStat)
{
	for (TFieldIterator<FNumericProperty> PropIt(
		     FABCharacterStat::StaticStruct());
	     PropIt != nullptr;
	     ++PropIt)
	{
		// 속성 키 값
		const FName PropKey(PropIt->GetName());

		// 리플렉션을 활용해 변수의 값 읽어오기
		float BaseData = 0.0f;
		PropIt->GetValue_InContainer((const void*)&BaseStat, &BaseData);

		// 리플렉션을 활용해 변수의 값 읽어오기
		float ModifierData = 0.0f;
		PropIt->GetValue_InContainer((const void*)&ModifierStat, &ModifierData);

		// 맵에서 Base 텍스트 블록을 가져와 값 설정
		UTextBlock** BaseTextBlockPtr = BaseLookup.Find(PropKey);
		if (BaseTextBlockPtr)
		{
			// Float 값을 FString으로 변환
			FString FloatValue = FString::SanitizeFloat(BaseData);

			// FString을 FText로 변환해서 텍스트 블록에 설정
			(*BaseTextBlockPtr)->SetText(FText::FromString(FloatValue));
		}
		
		// 맵에서 Modifier 텍스트 블록을 가져와 값 설정
		UTextBlock** ModifierTextBlockPtr = ModifierLookup.Find(PropKey);
		if (ModifierTextBlockPtr)
		{
			// Float 값을 FString으로 변환
			FString FloatValue = FString::SanitizeFloat(ModifierData);

			// FString을 FText로 변환해서 텍스트 블록에 설정
			(*ModifierTextBlockPtr)->SetText(FText::FromString(FloatValue));
		}
	}
}
