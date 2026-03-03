#include "MineItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AMineItem::AMineItem()
{
    ExplosionDelay = 5.0f;
    ExplosionRadius = 150.0f;
    ExplosionDamage = 30.0f;
    ItemType = "Mine";

    ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
    ExplosionCollision->InitSphereRadius(ExplosionRadius);
    ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    ExplosionCollision->SetupAttachment(Scene);
}

void AMineItem::ActivateItem(AActor* Activator)
{
    // 5초 후 폭발 실행
    GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AMineItem::Explode, ExplosionDelay);
}

void AMineItem::Explode()
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