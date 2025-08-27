// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/CHShip.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Character/CHPlayerCharacter.h"
#include "Controller/CHPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"


ACHShip::ACHShip()
{
    // Tick 사용 안 함 (입력 이벤트 때만 처리)
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicateMovement(true);

    // --- Components ---
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);
    Mesh->SetSimulatePhysics(false);
    Mesh->SetMobility(EComponentMobility::Movable);


    
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 2000.f;
    SpringArm->bUsePawnControlRotation = true;
    SpringArm->bDoCollisionTest = true; // 필요에 따라 false

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);

    AttachPoint = CreateDefaultSubobject<USceneComponent>(TEXT("AttachPoint"));
    AttachPoint->SetupAttachment(RootComponent);

    Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
    Movement->MaxSpeed     = 1200.f;
    Movement->Acceleration = 2000.f;
    Movement->Deceleration = 1500.f;
    Movement->TurningBoost = 0.f;

    // XY 평면 이동 고정
    Movement->SetPlaneConstraintEnabled(true);
    Movement->SetPlaneConstraintNormal(FVector::UpVector);
    Movement->bConstrainToPlane = true;
    Movement->bSnapToPlaneAtStart = true;

    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Movement->SetUpdatedComponent(RootComponent);
}

void ACHShip::BeginPlay()
{
    Super::BeginPlay();

    // BP에서 Root 바뀐 경우 대비
    if (Movement)
        Movement->SetUpdatedComponent(RootComponent);

    FixedZ = GetActorLocation().Z;

    Movement->SetUpdatedComponent(RootComponent);
}

void ACHShip::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (APlayerController* PC = Cast<APlayerController>(NewController))
    {
        // 시점 전환(선택)
        PC->ClientSetViewTarget(this);
    }
}

void ACHShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACHShip::OnMove);
        EIC->BindAction(MoveAction, ETriggerEvent::Completed, this, &ACHShip::OnMoveCompleted);
    }
}

void ACHShip::OnMove(const FInputActionValue& Value)
{
    const FVector2D Ax = Value.Get<FVector2D>();

    if (IsLocallyControlled())
    {
        // Tick 없이: 입력 들어온 프레임마다 서버가 즉시 1프레임치 처리
        Server_SetMoveInput(Ax);
    }
}

void ACHShip::OnMoveCompleted(const FInputActionValue& /*Value*/)
{
    if (IsLocallyControlled())
    {
        Server_SetMoveInput(FVector2D::ZeroVector);
    }
}

void ACHShip::Server_SetMoveInput_Implementation(FVector2D InMove)
{
    FVector2D Ax = InMove;
    if (bSwizzleYX) { Swap(Ax.X, Ax.Y); }
    if (bInvertForward) { Ax.Y = -Ax.Y; }
    ServerMoveInput = Ax;

    const float DT = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.f;

    // 컨트롤러 Yaw 기준 수평 벡터
    float ControlYaw = 0.f;
    if (const AController* C = GetController()) ControlYaw = C->GetControlRotation().Yaw;
    const FRotator YawRot(0.f, ControlYaw, 0.f);
    const FVector Fwd   = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
    const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

    // --------- 앞으로 가는 부분을 "직접" 주입 ----------
    if (Movement && !FMath::IsNearlyZero(Ax.Y))
    {
        // 표준 경로(주석 처리): AddMovementInput(Fwd, Ax.Y);
        // 확실한 경로(권장): Movement에 직접 입력 벡터 추가
        Movement->AddInputVector(Fwd * Ax.Y);

        // 디버그: 현재 속도 출력
        const float Spd = GetVelocity().Size();
        UE_LOG(LogTemp, Warning, TEXT("[Ship] Y=%+.2f  Speed=%.1f  Fwd=(%.2f,%.2f,%.2f)"),
               Ax.Y, Spd, Fwd.X, Fwd.Y, Fwd.Z);
    }

    // 회전(오른쪽키=+ → 오른쪽으로 돌게 부호 반전 유지)
    const float MaxSpd     = Movement ? Movement->MaxSpeed : 1200.f;
    const float Speed      = GetVelocity().Size();
    const float SpeedRatio = FMath::Clamp(MaxSpd > 1.f ? (Speed/MaxSpd) : 1.f, 0.35f, 1.f);
    const float YawDelta   = -Ax.X * YawTurnRateDegPerSec * SpeedRatio * DT;
    if (!FMath::IsNearlyZero(YawDelta))
        AddActorWorldRotation(FRotator(0.f, YawDelta, 0.f), false, nullptr, ETeleportType::None);

    // 사이드 슬립 감쇠(선택)
    if (SideFriction > 0.f && Movement)
    {
        const float SideV = FVector::DotProduct(GetVelocity(), Right);
        const float K     = SideFriction * 0.2f * DT;
        Movement->AddInputVector(-Right * SideV * K);
    }

    // Z 고정(백업)
    FVector L = GetActorLocation();
    L.Z = FixedZ;
    SetActorLocation(L, false, nullptr, ETeleportType::None);
}

// 표준 파이프라인 연결(AddMovementInput -> Movement)
UPawnMovementComponent* ACHShip::GetMovementComponent() const
{
    return Movement;
}

void ACHShip::Interact(AActor* Interactor)
{
    if (!HasAuthority()) return;

    APawn* Pawn = Cast<APawn>(Interactor);
    if (!Pawn) return;

    if (ACHPlayerCharacter* PCha = Cast<ACHPlayerCharacter>(Interactor))
    {
        PCha->AttachToComponent(AttachPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        if (UCharacterMovementComponent* CM = PCha->GetCharacterMovement())
        {
            CM->StopMovementImmediately();
        }
    }

    if (ACHPlayerController* PC = Cast<ACHPlayerController>(Pawn->GetController()))
    {
        PC->ServerRPC_PossessShip(this);
        SetOwner(PC);
    }
}

// --- Replication ---
void ACHShip::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ACHShip, ServerMoveInput);
}