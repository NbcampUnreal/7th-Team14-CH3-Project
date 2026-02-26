// Fill out your copyright notice in the Description page of Project Settings.


#include "Floor3.h"



// Sets default values
AFloor3::AFloor3()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(SceneRoot);

	/*static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/MapTemplates/SM_Template_Map_Floor.SM_Template_Map_Floor"));
	if (MeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(MeshAsset.Object);

	}

	static ConstructorHelpers::FObjectFinder<UMaterial>MaterialAsset(TEXT("/Engine/OpenWorldTemplate/LandscapeMaterial/MI_ProcGrid.MI_ProcGrid"));
	if (MaterialAsset.Succeeded())
	{
		StaticMeshComp->SetMaterial(0, MaterialAsset.Object);
	}*/

	PrimaryActorTick.bCanEverTick = true;
	RotationSpeed = 90.0f;
}

void AFloor3::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// Called when the game starts or when spawned
void AFloor3::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation();
	

	OnItemPickedUp();
}
 
// Called every frame
void AFloor3::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!FMath::IsNearlyZero(RotationSpeed))
	{
		AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
	}
	const FVector Current = GetActorLocation();
	const FVector MoveAxis = FVector::RightVector; // 월드 Y(좌우)

	// Start 기준으로 축 방향으로 얼마나 이동했는지(편도 거리 측정)
	const float MovedOnAxis = FVector::DotProduct(Current - StartLocation, MoveAxis);

	// 끝에 닿으면 방향 반전
	if (FMath::Abs(MovedOnAxis) >= MaxRange)
	{
		DirectionSign *= -1.f;
	}

	const FVector DeltaMove = MoveAxis * DirectionSign * MoveSpeed * DeltaTime;
	SetActorLocation(Current + DeltaMove);

}

void AFloor3::ResetActorPosition()
{
	SetActorLocation(FVector::ZeroVector);
}

float AFloor3::GetRotationSpeed() const
{
	return RotationSpeed;
}

void AFloor3::Destroyed()
{
	UE_LOG(LogTemp, Warning, TEXT("%s Destroyed"), *GetName());
	Super::Destroyed();
}

void AFloor3::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogTemp, Warning, TEXT("%s EndPlay"), *GetName());

	Super::EndPlay(EndPlayReason);
}





