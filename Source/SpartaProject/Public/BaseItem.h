#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemInterface.h"
#include "BaseItem.generated.h"

class USphereComponent;

UCLASS()
class SPARTAPROJECT_API ABaseItem : public AActor, public IItemInterface
{
    GENERATED_BODY()

public:
    ABaseItem();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FName ItemType;
    // 루트 컴포넌트 (씬)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
    USceneComponent* Scene;
    // 충돌 컴포넌트 (플레이어 진입 범위 감지용)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
    USphereComponent* Collision;
    // 아이템 시각 표현용 스태틱 메시
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
    UStaticMeshComponent* StaticMesh;

    virtual void OnItemOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult) override;
    virtual void OnItemEndOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex) override;
    virtual void ActivateItem(AActor* Activator) override;
    virtual FName GetItemType() const override;

    void DestroyItem();
};