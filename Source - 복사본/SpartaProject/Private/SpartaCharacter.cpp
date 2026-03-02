#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "EnhancedInputSubsystems.h"


ASpartaCharacter::ASpartaCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

    PlayerMeshInitialization();
    CameraInitialization();
    InputInitialization();
    WeaponMeshInitialization();

   static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstance(TEXT("/Game/BluePrints/ABP_Sparta.ABP_Sparta"));

   if (AnimInstance.Class)
   {
        GetMesh()->SetAnimInstanceClass(AnimInstance.Class);
   }

   // 💡 1. 폰(Pawn) 자체가 카메라의 회전을 똑같이 따라가지 않도록 설정합니다.
   bUseControllerRotationPitch = false;
   bUseControllerRotationYaw = false;
   bUseControllerRotationRoll = false;

   // 💡 2. 캐릭터가 이동하는 방향으로 자연스럽게 몸을 회전하도록 만듭니다.
   GetCharacterMovement()->bOrientRotationToMovement = true;

   // 💡 3. 회전 속도를 설정합니다. (Y값이 초당 회전 각도입니다)
   GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);


    NormalSpeed = 900.0f;
    SprintSpeedMultiplier = 1.5f;
    SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

    GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

    HPMax = 100.0f;
    HP = HPMax;
}

void ASpartaCharacter::BeginPlay()
{
    Super::BeginPlay();

    // 플레이어 컨트롤러를 가져와서 향상된 입력 서브시스템에 매핑 컨텍스트를 추가합니다.
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            if (DefaultContext)
            {
                // 0은 우선순위(Priority)를 의미합니다.
                Subsystem->AddMappingContext(DefaultContext, 0);
            }
        }
    }
}

void ASpartaCharacter::Tick(float DeltaTime)
{
}




void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Move, Look, Attack은 캐릭터 클래스의 변수(this->MoveAction 등)를 직접 사용합니다!
        if (MoveAction)
        {
            EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::Move);
        }

        if (LookAction)
        {
            EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::Look);
        }

        if (AttackAction)
        {
            EnhancedInput->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::Attack);
        }

        // Jump와 Sprint는 컨트롤러에만 선언해 두셨다면 기존처럼 Controller의 것을 사용해도 무방합니다.
        if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
        {
            if (PlayerController->JumpAction)
            {
                EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::StartJump);
                EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &ASpartaCharacter::StopJump);
            }

            if (PlayerController->SprintAction)
            {
                EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::StartSprint);
                EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed, this, &ASpartaCharacter::StopSprint);
            }
        }
    }
}

void ASpartaCharacter::PlayerMeshInitialization()
{
    ConstructorHelpers::FObjectFinder<USkeletalMesh>
        PlayerSkeletalMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Resources/Characters/Meshes/SKM_Manny.SKM_Manny'"));
    if (PlayerSkeletalMesh.Succeeded())
    {
        GetMesh()->SetSkeletalMesh(PlayerSkeletalMesh.Object);
        GetMesh()->SetWorldLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
    }
}

void ASpartaCharacter::WeaponMeshInitialization()
{
    WeaponStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("gun"));

    // 처음부터 Mesh의 소켓에 부착하도록 설정
    WeaponStaticMesh->SetupAttachment(GetMesh(), TEXT("Weapon_Socket"));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> weapon(TEXT("/Script/Engine.StaticMesh'/Game/FP_AKS74U_Animation/AKS74U/Meshes/AK74U.AK74U'"));

    if (weapon.Succeeded())
    {
        WeaponStaticMesh->SetStaticMesh(weapon.Object);
    }

    // 예시: WeaponStaticMesh(총기)에 부속품(조준경 등)을 붙이는 경우
    MagazineStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Magazine"));

    // 캐릭터의 Mesh가 아니라 'WeaponStaticMesh'를 부모로 지정해야 합니다.
    MagazineStaticMesh->SetupAttachment(WeaponStaticMesh, TEXT("Magazine_Socket"));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> mag(TEXT("/Script/Engine.StaticMesh'/Game/FP_AKS74U_Animation/AKS74U/Meshes/SM_AKS74U_Magazine.SM_AKS74U_Magazine'"));
    if (mag.Succeeded())
    {
        MagazineStaticMesh->SetStaticMesh(mag.Object);
    }
}

void ASpartaCharacter::CameraInitialization()
{
    bUseControllerRotationYaw = true;

    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (MoveComp)
    {
        MoveComp->bOrientRotationToMovement = true;
    }

    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    if (SpringArmComp)
    {
        SpringArmComp->SetupAttachment(RootComponent);
        SpringArmComp->SetWorldLocation(FVector(0, 0, 40));
        SpringArmComp->TargetArmLength = 250;
        SpringArmComp->SocketOffset = FVector(0, 40, 0);
        SpringArmComp->bUsePawnControlRotation = true;
    }

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    if (CameraComp)
    {
        CameraComp->SetupAttachment(SpringArmComp);
    }
}

void ASpartaCharacter::InputInitialization()
{
    static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputContext(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Inputs/IMC_Character.IMC_Character'"));
    if (InputContext.Object != nullptr)
    {
		DefaultContext = InputContext.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputMove(TEXT("/Script/EnhancedInput.InputAction'/Game/Inputs/IA_Move.IA_Move'"));
	if (InputMove.Object != nullptr)
	{
		MoveAction = InputMove.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputLook(TEXT("/Script/EnhancedInput.InputAction'/Game/Inputs/IA_Look.IA_Look'"));
	if (InputLook.Object != nullptr)
	{
		LookAction = InputLook.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputAttack(TEXT("/Script/EnhancedInput.InputAction'/Game/Inputs/IA_Attack.IA_Attack'"));
	if (InputAttack.Object != nullptr)
	{
		AttackAction = InputAttack.Object;
	}
}

void ASpartaCharacter::Move(const FInputActionValue& value)
{
    FVector2D MovementVector = value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // 1. 현재 카메라(컨트롤러)가 바라보는 방향의 회전값을 가져옵니다.
        const FRotator Rotation = Controller->GetControlRotation();

        // 2. 위/아래(Pitch), 기울기(Roll)는 무시하고 좌우 방향(Yaw)만 추출합니다.
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // 3. 카메라 방향을 기준으로 현실 세계의 '앞'과 '오른쪽' 벡터를 수학적으로 계산합니다.
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        // 4. 계산된 방향으로 입력값을 더해줍니다.
        AddMovementInput(ForwardDirection, MovementVector.Y); // W, S (앞, 뒤)
        AddMovementInput(RightDirection, MovementVector.X);  // A, D (좌, 우)
    }
}

void ASpartaCharacter::StartJump(const FInputActionValue& value)
{
    // Jump 함수는 Character가 기본 제공
    if (value.Get<bool>())
    {
        Jump();
    }
}

void ASpartaCharacter::StopJump(const FInputActionValue& value)
{
    // StopJumping 함수도 Character가 기본 제공
    if (!value.Get<bool>())
    {
        StopJumping();
    }
}

void ASpartaCharacter::Look(const FInputActionValue& value)
{
    // 마우스의 X, Y 움직임을 2D 축으로 가져옴
    FVector2D LookInput = value.Get<FVector2D>();

    // X는 좌우 회전 (Yaw), Y는 상하 회전 (Pitch)
    // 좌우 회전
    AddControllerYawInput(LookInput.X);
    // 상하 회전
    AddControllerPitchInput(LookInput.Y);
}

void ASpartaCharacter::StartSprint(const FInputActionValue& value)
{
    // Shift 키를 누른 순간 이 함수가 호출된다고 가정
    // 스프린트 속도를 적용
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
    }
}

void ASpartaCharacter::StopSprint(const FInputActionValue& value)
{
    // Shift 키를 뗀 순간 이 함수가 호출
    // 평상시 속도로 복귀
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
    }
}

void ASpartaCharacter::Attack(const FInputActionValue& value)
{
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Attack"));

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

    if (AnimInstance)
    {
        if (AttackMontage)
        {
            AnimInstance->Montage_Play(AttackMontage);
        }
    }
}

float ASpartaCharacter::GetHP() const
{
    return HP;
}

void ASpartaCharacter::AddHP(float Amount)
{
    HP = FMath::Clamp(HP + Amount, 0.0f, HPMax);
    UE_LOG(LogTemp, Warning, TEXT("HP increased to: %f"), HP);
}

float ASpartaCharacter::TakeDamage(
    float DamageAmount,
    struct FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    HP = FMath::Clamp(HP - DamageAmount, 0.0f, HPMax);
    UE_LOG(LogTemp, Warning, TEXT("HP decreased to: %f"), HP);

    if (HP <= 0.0f)
    {
        OnDeath();
    }

    return ActualDamage;
}

void ASpartaCharacter::OnDeath()
{
    //게임 종료 로직
}