// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AGhostItem::AGhostItem()
{
    ExplosionDelay = 0.5f;
    ExplosionRadius = 300.0f;
    ExplosionDamage = 50.0f;
    ItemType = "Ghost";

    ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
    ExplosionCollision->InitSphereRadius(ExplosionRadius);
    ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    ExplosionCollision->SetupAttachment(Scene);
}

void AGhostItem::BeginPlay()
{
    Super::BeginPlay();

    // 시작할 때 체력을 꽉 채워줍니다.
    HP = MaxHP;
}

void AGhostItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 1. 플레이어인지 확인
    if (OtherActor && OtherActor->ActorHasTag("Player"))
    {
        // 2. 즉시 데미지 적용
        UGameplayStatics::ApplyDamage(
            OtherActor,           // 데미지를 받을 대상
            10.0f,                // 데미지 양
            nullptr,      // 데미지를 준 가해자(유령)의 컨트롤러
            this,                 // 데미지를 준 원인 액터
            UDamageType::StaticClass()
        );
    }
}

// 데미지를 입었을 때 실행되는 로직
float AGhostItem::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // 부모(AActor)의 TakeDamage 실행
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    return ActualDamage;
}

void AGhostItem::ActivateItem(AActor* Activator)
{
    // 5초 후 폭발 실행
    GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AGhostItem::Explode, ExplosionDelay);
}

void AGhostItem::Explode()
{
    // 유령 제거
    DestroyItem();
}