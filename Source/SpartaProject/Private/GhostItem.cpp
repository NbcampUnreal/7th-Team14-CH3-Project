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

// 데미지를 입었을 때 실행되는 로직
float AGhostItem::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // 부모(AActor)의 TakeDamage 실행
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // 데미지만큼 체력 감소
    HP -= ActualDamage;

    // 체력이 0 이하가 되면 폭발
    if (HP <= 0.0f)
    {
        // 원한다면 타이머를 취소하고 즉시 폭발하게 할 수 있습니다.
        GetWorld()->GetTimerManager().ClearTimer(ExplosionTimerHandle);
        Explode();
    }

    return ActualDamage;
}

void AGhostItem::ActivateItem(AActor* Activator)
{
    // 5초 후 폭발 실행
    GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AGhostItem::Explode, ExplosionDelay);
}

void AGhostItem::Explode()
{
    TArray<AActor*> OverlappingActors;
    ExplosionCollision->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (Actor && Actor->ActorHasTag("Player"))
        {
            UGameplayStatics::ApplyDamage(
                Actor, ExplosionDamage, nullptr, this,
                UDamageType::StaticClass()
            );
        }
    }

    // 유령 제거
    DestroyItem();
}