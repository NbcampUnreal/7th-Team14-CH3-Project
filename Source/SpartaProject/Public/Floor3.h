// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Floor3.generated.h"

UENUM(BlueprintType)
enum class EMoveAxisMode : uint8
{
	WorldX       UMETA(DisplayName = "World X"),
	ActorForward UMETA(DisplayName = "Actor Forward")
};

UCLASS()
class SPARTAPROJECT_API AFloor3 : public AActor
{
	GENERATED_BODY()

public:
	AFloor3();

protected:
	// Called when the game starts or when spawned
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Floor3|Components")
	USceneComponent* SceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Settings")
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Floor3|Properties")
	float RotationSpeed;

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float Deltatime) override;

	UFUNCTION(BlueprintCallable, Category = "Floor3|Actions")
	void ResetActorPosition();

	UFUNCTION(BlueprintPure, Category = "Floor3|Properties")
	float GetRotationSpeed() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor3|Event")
	void OnItemPickedUp();

	virtual void Destroyed() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	FVector StartLocation;

	// 축 선택: 둘 다 지원하려고 enum으로 분기
	

	UPROPERTY(EditAnywhere, Category = "Floor3|Move")
	EMoveAxisMode MoveAxisMode = EMoveAxisMode::ActorForward;

	UPROPERTY(EditAnywhere, Category = "Floor3|Move")
	float MoveSpeed = 200.f;

	// StartLocation 기준 "편도" 거리
	UPROPERTY(EditAnywhere, Category = "Floor3|Move")
	float MaxRange = 300.f;

	float DirectionSign = 1.f;

};
