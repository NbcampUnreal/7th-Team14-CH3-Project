// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GhostItem.generated.h"

UCLASS()
class SPARTAPROJECT_API AGhostItem : public ABaseItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGhostItem();

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetHP() const;

    UFUNCTION(BlueprintCallable, Category = "Health")
    void AddHP(float Amount);

    // 공격받아 죽었을 때 호출
    void OnDeath();

protected:
    virtual void BeginPlay() override;

    // --- AI 및 이동 관련 ---
    UPROPERTY(EditAnywhere, Category = "Ghost AI")
    float WanderRadius = 500.0f;
    UPROPERTY(EditAnywhere, Category = "Ghost AI")
    float MoveSpeed = 150.0f;
    UPROPERTY(EditAnywhere, Category = "Ghost AI")
    float WaitTime = 2.0f;

    // --- 체력 관련 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP")
    float HPMax = 100.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP")
    float HP;

    // 폭발까지 걸리는 시간 (5초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mine")
    float ExplosionDelay;
    // 폭발 범위
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mine")
    float ExplosionRadius;
    // 폭발 데미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mine")
    int ExplosionDamage;

    // 지뢰 발동 여부
    FTimerHandle ExplosionTimerHandle;

    virtual void ActivateItem(AActor* Activator) override;

    void Explode();

private:
    FVector OriginLocation;
    FVector TargetLocation;
    EGhostState CurrentState;
    float StateTimer;
    float FloatingTime;

    void GetNewRandomLocation();
};