#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/StaticMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "Logging/LogMacros.h"
#include "SpartaCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class SPARTAPROJECT_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASpartaCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputMappingContext* DefaultContext;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* AttackAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	// 속도에 따른 팔 길이 조절을 위한 설정값
	UPROPERTY(EditAnywhere, Category = "Camera")
	float MinArmLength = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float MaxArmLength = 800.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float ZoomInterpSpeed = 2.0f; // 변화 속도

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHP() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHP(float Amount);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float NormalSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeedMultiplier;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP")
	float HPMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP")
	float HP;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

private:
	void PlayerMeshInitialization();
	void WeaponMeshInitialization();
	void CameraInitialization();
	void InputInitialization();

	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);

	UFUNCTION()
	void Attack(const FInputActionValue& value);

	void OnDeath();

private:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* WeaponStaticMesh;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MagazineStaticMesh;

private:
	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage;
};
