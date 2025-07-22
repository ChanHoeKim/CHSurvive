// Copyright Epic Games, Inc. All Rights Reserved.

#include "CHPlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/CHPlayerCharacter.h"

#include "CHDefine.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Component/CHCombatComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/CHInteractInterface.h"
#include "Kismet/KismetSystemLibrary.h"

ACHPlayerCharacter::ACHPlayerCharacter()
{
	// Set size for player capsule
	//GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bReplicates = true;
	
	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (Movement)
	{
		Movement->MaxWalkSpeed = CharacterMaxWalkSpeed;
	}
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 2000.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	CombatComponent = CreateDefaultSubobject<UCHCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);
	
}

void ACHPlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (bShouldMove)
	{
		FVector ShouldMoveDirection = TargetPoint - GetActorLocation();
		ShouldMoveDirection.Z = 0;
		float Distance = ShouldMoveDirection.Size();

		if (Distance > MoveTargetAllowRadius)
		{
			AddMovementInput(ShouldMoveDirection.GetSafeNormal());
		}
		else
		{
			bShouldMove = false;
		}
	}
}

void ACHPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Add Input Mapping Context
	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Triggered, this, &ACHPlayerCharacter::OnClickMove);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ACHPlayerCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACHPlayerCharacter::SprintEnd);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ACHPlayerCharacter::Interact);
		
	}
	else
	{
		if (GEngine)
		{
				GEngine->AddOnScreenDebugMessage(
						-1, // Key (고유 ID, -1이면 자동으로 갱신됨)
							5.0f, // Duration (화면에 표시될 시간, 초 단위)
								FColor::Green, // 텍스트 색상
									TEXT("키 바인딩 에러") // 출력할 메시지
									);
									}
	}
}

void ACHPlayerCharacter::OnClickMove()
{
	// if (GEngine)
	// {
	// 		GEngine->AddOnScreenDebugMessage(
	// 				-1, // Key (고유 ID, -1이면 자동으로 갱신됨)
	// 					5.0f, // Duration (화면에 표시될 시간, 초 단위)
	// 						FColor::Green, // 텍스트 색상
	// 							TEXT("Click") // 출력할 메시지
	// 							);
	// 							}

	FHitResult Hit;
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	}
	
	if (Hit.bBlockingHit)
	{
		TargetPoint = Hit.Location;
		bShouldMove = true;
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			FXCursor,
			Hit.Location,
			FRotator::ZeroRotator,
			FVector(1.f, 1.f, 1.f),
			true,
			true,
			ENCPoolMethod::None,
			true);
	}
}

void ACHPlayerCharacter::CheckInteract()
{
	FHitResult OutHit;
	const FVector Start = GetActorLocation();
	const FVector End = Start;
	constexpr float Radius = 200.f;
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_OBJECT_INTERACTION));
	
	TArray<AActor*> ActorsToIgnore;
	
	bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
			this,
			Start,
			End,
			Radius,
			ObjectTypes,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForDuration,
			OutHit,
			true
	);
	
	if (bHit)
	{
		if (AActor* HitActor = OutHit.GetActor())
		{
			if (ICHInteractInterface* Interaction = Cast<ICHInteractInterface>(HitActor))
			{
				Interaction->Interact(this);
			}
		}
	}
}

void ACHPlayerCharacter::Interact()
{
	if (HasAuthority()) // 서버에서만 실행
	{
		CheckInteract();
	}
	else
	{
		ServerInteract();
	}
}

void ACHPlayerCharacter::ServerInteract_Implementation()
{
	if (!HasAuthority()) return;
	
	// 내 캐릭터가 맞으면 서버에 상호작용 요청 RPC 호출
	CheckInteract();
}

void ACHPlayerCharacter::Sprint()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
				-1, // Key (고유 ID, -1이면 자동으로 갱신됨)
					5.0f, // Duration (화면에 표시될 시간, 초 단위)
						FColor::Green, // 텍스트 색상
							TEXT("Click") // 출력할 메시지
							);
	}
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (Movement)
	{
		ServerRPCChangeCharacterMaxWalkSpeed(2000.f);
	}
}

void ACHPlayerCharacter::SprintEnd()
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (Movement)
	{
		ServerRPCChangeCharacterMaxWalkSpeed(700.f);
	}
}

void ACHPlayerCharacter::ServerRPCChangeCharacterMaxWalkSpeed_Implementation(float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}