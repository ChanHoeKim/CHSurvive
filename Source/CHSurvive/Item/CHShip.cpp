// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/CHShip.h"

#include "EngineUtils.h"
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

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    // 충돌 및 물리 기본 설정
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Mesh->SetCollisionObjectType(COLLISION_OBJECT_INTERACTION);
}

void ACHShip::BeginPlay()
{
    Super::BeginPlay();
    
}


void ACHShip::Interact(AActor* Interactor)
{
    if (!HasAuthority()) return;

    ACHPlayerCharacter* Character = Cast<ACHPlayerCharacter>(Interactor);
    
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        FString DisplayName = It->GetActorNameOrLabel(); // UE 5.1+
        if (DisplayName == TeleportName)
        {
            FVector Loc = It->GetActorLocation();
            FRotator Rot = It->GetActorRotation();
            UE_LOG(LogTemp, Warning, TEXT("Found %s at %s"), *DisplayName, *Loc.ToString());
            Character->StopMove();
            Character->TeleportTo(Loc, Rot);
        }
    }
}
