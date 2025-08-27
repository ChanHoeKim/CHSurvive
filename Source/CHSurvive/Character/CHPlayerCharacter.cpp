// Copyright Epic Games, Inc. All Rights Reserved.

#include "CHPlayerCharacter.h"

#include "AIController.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"

#include "CHDefine.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Animation/CHAnimInstance.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Component/CHCombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/LocalPlayer.h"
#include "Environment/CHTree.h"
#include "Equipment/CHWeapon.h"
#include "Interface/CHInteractInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "UI/CHInventoryWidget.h"

ACHPlayerCharacter::ACHPlayerCharacter()
{
	// Set size for player capsule
	//GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bReplicates = true;
	SetReplicateMovement(true);
	
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

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void ACHPlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	// 서버이면서 로컬이 아닌(=원격 플레이어의 서버 권위) 경우, 또는 소유 클라에서만 실행
	const bool bServerNonLocal = HasAuthority() && !IsLocallyControlled();
	if (!(IsLocallyControlled() || bServerNonLocal)) return;
	
	if (!bFollowingPath || !FollowPath.IsValidIndex(PathIndex)) return;

	const FVector Here   = GetActorLocation();
	FVector Target       = FollowPath[PathIndex];
	Target.Z             = Here.Z;

	const FVector To     = Target - Here;
	const float Dist2D   = To.Size2D();

	if (Dist2D <= AcceptanceRadius)
	{
		++PathIndex;
		if (!FollowPath.IsValidIndex(PathIndex))
		{
			bFollowingPath = false;
		}
		return;
	}

	AddMovementInput(To.GetSafeNormal());
}

void ACHPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (InventoryWidgetClass)
	{
		InventoryWidget = CreateWidget<UUserWidget>(GetWorld(), InventoryWidgetClass);

		InventoryWidget->AddToViewport();
		InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ACHPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (auto* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent)){
		EIC->BindAction(ClickAction, ETriggerEvent::Triggered, this, &ACHPlayerCharacter::OnClickMove);
		EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &ACHPlayerCharacter::Sprint);
		EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACHPlayerCharacter::SprintEnd);
		EIC->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ACHPlayerCharacter::Interact);
		EIC->BindAction(ReadyToAttackAction, ETriggerEvent::Started, this, &ACHPlayerCharacter::ReadyToAttack);
		EIC->BindAction(ReadyToAttackAction, ETriggerEvent::Completed, this, &ACHPlayerCharacter::ReadyToAttackEnd);
		EIC->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ACHPlayerCharacter::Attack);

		EIC->BindAction(OpenInventoryAction, ETriggerEvent::Triggered, this, &ACHPlayerCharacter::OpenInventory);
	}
	
	// // Add Input Mapping Context
	// APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	// UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	// if (Subsystem)
	// {
	// 	Subsystem->AddMappingContext(DefaultMappingContext, 0);
	// }
	//
	// // Set up action bindings
	// if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	// {
	//
	// 	EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Triggered, this, &ACHPlayerCharacter::OnClickMove);
	// 	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ACHPlayerCharacter::Sprint);
	// 	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACHPlayerCharacter::SprintEnd);
	// 	EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ACHPlayerCharacter::Interact);
	// 	EnhancedInputComponent->BindAction(ReadyToAttackAction, ETriggerEvent::Started, this, &ACHPlayerCharacter::ReadyToAttack);
	// 	EnhancedInputComponent->BindAction(ReadyToAttackAction, ETriggerEvent::Completed, this, &ACHPlayerCharacter::ReadyToAttackEnd);
	// 	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ACHPlayerCharacter::Attack);
	//
	// 	EnhancedInputComponent->BindAction(OpenInventoryAction, ETriggerEvent::Triggered, this, &ACHPlayerCharacter::OpenInventory);
	// }
	// else
	// {
	// 	if (GEngine)
	// 	{
	// 			GEngine->AddOnScreenDebugMessage(
	// 					-1, // Key (고유 ID, -1이면 자동으로 갱신됨)
	// 						5.0f, // Duration (화면에 표시될 시간, 초 단위)
	// 							FColor::Green, // 텍스트 색상
	// 								TEXT("키 바인딩 에러") // 출력할 메시지
	// 								);
	// 	}
	// }
}

void ACHPlayerCharacter::Client_StartFollow_Implementation(const TArray<FVector>& InPath)
{
	FollowPath    = InPath;
	PathIndex     = 0;
	bFollowingPath= FollowPath.Num() > 0;
}

void ACHPlayerCharacter::Server_RequestMove_Implementation(const FVector& RawHit)
{
	if (UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		FNavLocation Goal;
		bool CanGo = Nav->ProjectPointToNavigation(RawHit, Goal, FVector(300,300,400));
		if (!CanGo)
		{
			GEngine->AddOnScreenDebugMessage(
						-1, // Key (고유 ID, -1이면 자동으로 갱신됨)
							5.0f, // Duration (화면에 표시될 시간, 초 단위)
								FColor::Green, // 텍스트 색상
									TEXT("이동 불가") // 출력할 메시지
									);
			return;
		}

		//
		UNavigationPath* Path = Nav->FindPathToLocationSynchronously(
			GetWorld(), GetActorLocation(), Goal.Location);
		if (!Path || !Path->IsValid())
		{
			GEngine->AddOnScreenDebugMessage(
						-1, // Key (고유 ID, -1이면 자동으로 갱신됨)
							5.0f, // Duration (화면에 표시될 시간, 초 단위)
								FColor::Green, // 텍스트 색상
									TEXT("갈 곳 없음") // 출력할 메시지
									);
			return;
		}

		const TArray<FVector>& Pts = Path->PathPoints;
		if (Pts.Num() == 0)
		{
			GEngine->AddOnScreenDebugMessage(
						-1, // Key (고유 ID, -1이면 자동으로 갱신됨)
							5.0f, // Duration (화면에 표시될 시간, 초 단위)
								FColor::Green, // 텍스트 색상
									TEXT("경로 0") // 출력할 메시지
									);
			return;
		}

		//서버 쪽에도 경로/상태 세팅 (자기 자신도 따라가게)
		FollowPath.Reset();
		for (int32 i = 1; i < Pts.Num(); ++i) // [0]은 보통 현재 위치 근처
		{
			FollowPath.Add(Pts[i]);
		}
		if (FollowPath.Num() == 0)
		{
			FollowPath.Add(Goal.Location);
		}
		PathIndex      = 0;
		bFollowingPath = true;

		// 클라에도 시작 신호 전달
		Client_StartFollow(FollowPath);
	}
}

void ACHPlayerCharacter::Attack()
{
	
	if (!bBeReadyToAttack)
	{
		return;
	}
	
	if (!bCanAttack)
	{
		return;
	}
	bCanAttack = false;
	
	if (!HasAuthority())
	{
		AttackAnimationPlay();
	}
	
	ServerRPCAttack();
	
}

void ACHPlayerCharacter::AttackHitCheck()
{
	if (IsLocallyControlled())
	{
		ACHWeapon* Weapon = Cast<ACHWeapon>(CombatComponent->GetMainWeapon());
		UPrimitiveComponent* WeaponMesh = Cast<UPrimitiveComponent>(Weapon->Mesh);
		
		if (WeaponMesh)
		{
			const FVector Start =WeaponMesh->GetSocketLocation(Weapon->TraceStartSocketName);
			const FVector End =WeaponMesh->GetSocketLocation(Weapon->TraceEndSocketName);
			const float TraceRadius = 50.f;
			
			FHitResult OutHitResult;
			TArray <TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
			TArray<AActor*> IgnoredActors;
			TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType = EDrawDebugTrace::ForDuration;
			FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

			
			bool HitDetected = GetWorld()->SweepSingleByChannel(
				OutHitResult,
				Start,
				End,
				FQuat::Identity,
				ECC_Pawn,
				FCollisionShape::MakeSphere(TraceRadius),
				Params);
			
			if (HitDetected)
			{
				FDamageEvent DamageEvent;
				if (!HasAuthority())
				{
					ServerRPC_AttackHitCheck(OutHitResult.GetActor());
				}
				OutHitResult.GetActor()->TakeDamage(10, DamageEvent, GetController(), this);
				
				
			}

	#if ENABLE_DRAW_DEBUG

				FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
				float CapsuleHalfHeight = FVector::Dist(Start, End) * 0.5f;
				FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

				DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, TraceRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);

	#endif
			}
	}
}

void ACHPlayerCharacter::ServerRPC_AttackHitCheck_Implementation(AActor* InActor)
{
	FDamageEvent DamageEvent;
	InActor->TakeDamage(10, DamageEvent, GetController(), this);
}

void ACHPlayerCharacter::OnOffTagetAutoAttack(bool InOnOff)
{
	if (InOnOff)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(TargetAttackTimerHandle) == false)
		{
			GetWorld()->GetTimerManager().SetTimer(TargetAttackTimerHandle,this, &ACHPlayerCharacter::Attack, 0.5f, true);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(TargetAttackTimerHandle);
	}
}


void ACHPlayerCharacter::ServerRPCAttackTargetEnd_Implementation()
{
	bBeReadyToAttack = false;
}

void ACHPlayerCharacter::ServerRPCAttack_Implementation()
{
	AttackAnimationPlay();

	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController && GetController() != PlayerController)
		{
			if(!PlayerController->IsLocalController())
			{
				ACHPlayerCharacter* OtherPlayer = Cast<ACHPlayerCharacter>(PlayerController->GetPawn());
				if (OtherPlayer)
				{
					OtherPlayer->ClientRPCPlayAnimation(this);
				}
			}
		}
	}
}

void ACHPlayerCharacter::AttackAnimationPlay()
{
	ACHWeapon* Weapon = CombatComponent->GetMainWeapon();
	if (Weapon)
	{
		UAnimMontage* AttackAnim = Weapon->GetAttackAnim();
		if (AttackAnim)
		{
			
			UCHAnimInstance* AnimInstance = Cast<UCHAnimInstance>(GetMesh()->GetAnimInstance());
			if (AnimInstance)
			{
				AnimInstance->StopAllMontages(0.0f);
				AnimInstance->Montage_Play(AttackAnim);
			}
		}
	}
}

void ACHPlayerCharacter::ClientRPCPlayAnimation_Implementation(ACHPlayerCharacter* CharacterType)
{
	if (CharacterType)
	{
		CharacterType->AttackAnimationPlay();
	}
}



void ACHPlayerCharacter::ReadyToAttack()
{
	if (HasAuthority())
	{
		bBeReadyToAttack = true;
		bShouldMove = false;
		UCHAnimInstance* Anim = Cast<UCHAnimInstance>(GetMesh()->GetAnimInstance());
		if (Anim)
		{
			Anim->UpdateAttackMode();
		}
	}
	else
	{
		ServerRPCChangebBeReadyToAttack(true);
	}
}

void ACHPlayerCharacter::OpenInventory()
{
	check(InventoryWidget);
	
	//인벤토리 열린 상태
	if (bIsOpenInventory)
	{
		//InventoryWidget->RemoveFromParent();
		InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
		bIsOpenInventory = false;
	}
	//인벤토리 닫힌 상태
	else 
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Visible);
		//InventoryWidget->AddToViewport();
		bIsOpenInventory = true;
	}
}



void ACHPlayerCharacter::ReadyToAttackEnd()
{
	
	if (HasAuthority())
	{
		bBeReadyToAttack = false;
		UCHAnimInstance* Anim = Cast<UCHAnimInstance>(GetMesh()->GetAnimInstance());
		if (Anim)
		{
			Anim->UpdateAttackMode();
		}
	}
	else
	{
		ServerRPCChangebBeReadyToAttack(false);
	}
}

void ACHPlayerCharacter::ServerRPCChangebBeReadyToAttack_Implementation(bool InBool)
{
	bBeReadyToAttack = InBool;
	UCHAnimInstance* Anim = Cast<UCHAnimInstance>(GetMesh()->GetAnimInstance());
	if (Anim)
	{
		Anim->UpdateAttackMode();
	}
}

void ACHPlayerCharacter::OnRep_ChangebBeReadyToAttack()
{

	UCHAnimInstance* Anim = Cast<UCHAnimInstance>(GetMesh()->GetAnimInstance());
	if (Anim)
	{
		Anim->UpdateAttackMode();
	}
}

void ACHPlayerCharacter::OnClickMove()
{

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (!PC->IsLocalController()) return;

		FHitResult Hit;
		PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);
		if (!Hit.bBlockingHit) return;

		Server_RequestMove(Hit.ImpactPoint); // ★ 투영 없이 좌표만
	}
	
}

void ACHPlayerCharacter::CheckInteract()
{
	if (HasAuthority())
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

void ACHPlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentWeapon);
	DOREPLIFETIME(ThisClass, bBeReadyToAttack);
}




void ACHPlayerCharacter::OnRep_ChangeWeaponType()
{
	UCHAnimInstance* Anim = Cast<UCHAnimInstance>(GetMesh()->GetAnimInstance());
	if (Anim)
	{
		Anim->UpdateIdle();
	}
}

void ACHPlayerCharacter::ChangeWeaponType(EWeaponType WeaponType)
{
	// if (GEngine)
	// {
	// 		GEngine->AddOnScreenDebugMessage(
	// 				-1, // Key (고유 ID, -1이면 자동으로 갱신됨)
	// 					5.0f, // Duration (화면에 표시될 시간, 초 단위)
	// 						FColor::Green, // 텍스트 색상
	// 							TEXT("Click") // 출력할 메시지
	// 							);
	// }
	if (HasAuthority())
	{
		CurrentWeapon = WeaponType;
		OnRep_ChangeWeaponType();
	}
}

void ACHPlayerCharacter::ServerInteract_Implementation()
{
	if (!HasAuthority()) return;
	
	// 내 캐릭터가 맞으면 서버에 상호작용 요청 RPC 호출
	CheckInteract();
}

void ACHPlayerCharacter::ClientRPC_Interact_Implementation(ACHLog* InLog)
{
	UCHInventoryWidget* InventoryWidget1 = Cast<UCHInventoryWidget>(InventoryWidget);
	if (InventoryWidget1)
	{
		InventoryWidget1->GetLog(InLog);
	}
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
		ServerRPCChangeCharacterMaxWalkSpeed(1000.f);
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
