// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "GhostItem.generated.h"

/**
 * 
 */
UCLASS()
class SPARTAPROJECT_API AGhostItem : public ABaseItem
{
	GENERATED_BODY()

public:
    AGhostItem();

    // 아이템의 최대 체력과 현재 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ghost|Health")
    float MaxHP = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ghost|Health")
    float HP;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
    USphereComponent* ExplosionCollision;

    // 폭발까지 걸리는 시간 (5초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ghost")
    float ExplosionDelay;
    // 폭발 범위
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ghost")
    float ExplosionRadius;
    // 폭발 데미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ghost")
    int ExplosionDamage;

    // 지뢰 발동 여부
    FTimerHandle ExplosionTimerHandle;

    virtual void ActivateItem(AActor* Activator) override;

    void Explode();

    // 시작할 때 HP를 MaxHP로 초기화하기 위해 필요
    virtual void BeginPlay() override;

    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    // 데미지를 받을 때 호출되는 엔진 기본 함수 오버라이드
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
};
