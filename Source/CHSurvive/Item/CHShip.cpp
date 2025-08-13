// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/CHShip.h"

#include "Camera/CameraComponent.h"
#include "Character/CHPlayerCharacter.h"
#include "Controller/CHPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ACHShip::ACHShip()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);
	
	//bUseControllerRotationPitch = false;
	//bUseControllerRotationYaw = false;
	//bUseControllerRotationRoll = false;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);  // @실수 : 미지정해서 카메라가 이상한 곳에 붙었었음
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 2000.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bDoCollisionTest = true;  //충돌 무시
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Mesh->SetSimulatePhysics(false); //물리 켜져있으면 움직임 안 먹음
	
	// CPP
	AttachPoint = CreateDefaultSubobject<USceneComponent>(TEXT("AttachPoint"));
	//AttachPoint->SetupAttachment(RootComponent);
	//AttachPoint->SetRelativeLocation(FVector(0.f, 0.f, 100.f));

	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
}

// Called when the game starts or when spawned
void ACHShip::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACHShip::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		PC->ClientSetViewTarget(this);
	}
}

// Called every frame
void ACHShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACHShip::Interact(AActor* Interactor)
{
	if (!HasAuthority()) return;
	
	APawn* Pawn = Cast<APawn>(Interactor);
	 if (!Pawn) return;

	ACHPlayerCharacter* PCha = Cast<ACHPlayerCharacter>(Interactor);
	
	
	PCha->AttachToComponent(
	AttachPoint,
	FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	PCha->GetCharacterMovement()->StopMovementImmediately();
	
	ACHPlayerController* PC = Cast<ACHPlayerController>(Pawn->GetController());
	if (!PC) return;

	// APawn* ShipPawn = Cast<APawn>(this);
	// if (!ShipPawn) return;
	PC->ServerRPC_PossessShip(Cast<APawn>(this));
	SetOwner(PC);
}

