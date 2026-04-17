// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/ABFountain.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AABFountain::AABFountain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 컴포넌트 객체 생성.
	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Water"));

	// 루트 컴포넌트 지정.
	RootComponent = Body;

	// Water 컴포넌트는 루트의 하위 컴포넌트로 지정.
	Water->SetupAttachment(Body);
	// 하위 컴포넌트 설정한 후에는 상대 위치 설정해주는 게 좋음.
	Water->SetRelativeLocation(FVector(0.0f, 0.0f, 132.0f));

	// 분수대 애셋 검색 후 설정.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FountainBodyMesh(
		TEXT("/Game/ArenaBattle/Environment/Props/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01")
	);

	if (FountainBodyMesh.Succeeded())
	{
		Body->SetStaticMesh(FountainBodyMesh.Object);
	}

	// 물 애셋 검색 후 설정.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FountainWaterMesh(
		TEXT("/Game/ArenaBattle/Environment/Props/SM_Plains_Fountain_02.SM_Plains_Fountain_02")
	);

	if (FountainWaterMesh.Succeeded())
	{
		Water->SetStaticMesh(FountainWaterMesh.Object);
	}
}

// Called when the game starts or when spawned
void AABFountain::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AABFountain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

