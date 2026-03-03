// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AGhostItem::AGhostItem()
{
    ExplosionDelay = 5.0f;
    ExplosionRadius = 150.0f;
    ExplosionDamage = 30.0f;
    ItemType = "Ghost";

    ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
    ExplosionCollision->InitSphereRadius(ExplosionRadius);
    ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    ExplosionCollision->SetupAttachment(Scene);

    // 초기값 세팅
    HPMax = 100.0f;
    HP = HPMax;
}

void AGhostItem::Tick(float DeltaTime)
{
}

void AGhostItem::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
}

// Called when the game starts or when spawned
void AGhostItem::BeginPlay()
{
	Super::BeginPlay();
    OriginLocation = GetActorLocation();
    GetNewRandomLocation();
    CurrentState = EGhostState::Wandering;

    // 플레이어 감지를 위한 오버랩 이벤트 바인딩
    GhostMesh->OnComponentBeginOverlap.AddDynamic(this, &AGhostPawn::OnOverlapBegin);
}

void AGhostItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 플레이어와 처음 닿았을 때 폭발 시퀀스 시작
    if (OtherActor && OtherActor->ActorHasTag("Player") && !ExplosionTimerHandle.IsValid())
    {
        // 붉은색으로 변하거나 하는 연출을 여기에 추가 가능
        GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AGhostPawn::Explode, ExplosionDelay);
    }
}

void AGhostItem::ActivateItem(AActor* Activator)
{
}

void AGhostItem::Explode()
{
    // MineItem의 폭발 로직 적용
    TArray<AActor*> OverlappingActors;
    ExplosionCollision->SetSphereRadius(ExplosionRadius); // 폭발 시점에 범위 확정
    ExplosionCollision->GetOverlappingActors(OverlappingActors);

    // 폭발 이펙트 (필요 시 추가)
    // UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

    for (AActor* Actor : OverlappingActors)
    {
        if (Actor && Actor->ActorHasTag("Player"))
        {
            UGameplayStatics::ApplyDamage(Actor, ExplosionDamage, GetController(), this, UDamageType::StaticClass());
        }
    }

    // 자기 자신 제거 (성불/폭발)
    Destroy();
}

float AGhostItem::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    AddHP(-ActualDamage);

    if (HP <= 0.0f)
    {
        OnDeath();
    }
    return ActualDamage;
}

void AGhostItem::OnDeath()
{
    // 공격받아 죽었을 때: 폭발하지 않고 사라지게 하려면 그냥 Destroy()
    // 만약 죽을 때도 폭발하게 하려면 Explode() 호출
    Destroy();
}

// --- 나머지 이동/부유 로직(Tick 등)은 기존 코드와 동일하게 유지 ---
void AGhostItem::Tick(float DeltaTime) {
    Super::Tick(DeltaTime); FloatingTime += DeltaTime; FVector CurrentLoc = GetActorLocation(); 
    float BobbingEffect = FMath::Sin(FloatingTime * 2.0f) * 0.7f; CurrentLoc.Z += BobbingEffect; SetActorLocation(CurrentLoc); 
    if (CurrentState == EGhostState::Wandering) { 
        FVector Direction = (TargetLocation - GetActorLocation()); 
        float Distance = Direction.Size(); 
        if (Distance > 20.0f) { 
            FVector NewLoc = GetActorLocation() + (Direction.GetSafeNormal() * MoveSpeed * DeltaTime); 
            SetActorLocation(NewLoc); FRotator TargetRot = Direction.Rotation(); 
            SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 2.0f)); 
        } 
        else { 
            CurrentState = EGhostState::Idle; StateTimer = 0.0f; 
        } 
    } 
    else if (CurrentState == EGhostState::Idle) { 
        StateTimer += DeltaTime; if (StateTimer >= WaitTime) { 
            GetNewRandomLocation(); 
        } 
    } 
}
void AGhostItem::AddHP(float Amount) {
    HP = FMath::Clamp(HP + Amount, 0.0f, HPMax); 
}
float AGhostItem::GetHP() const {
    return HP; 
}

void AGhostItem::GetNewRandomLocation() {
    FVector RandomOffset = FMath::VRand() * FMath::FRandRange(0.0f, WanderRadius); 
    TargetLocation = OriginLocation + RandomOffset;
    CurrentState = EGhostState::Wandering; 
}
void AGhostItem::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
    Super::SetupPlayerInputComponent(PlayerInputComponent); 
}
