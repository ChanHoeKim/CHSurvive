// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CHAnimInstance.h"
#include "Character/CHPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


void UCHAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Character = Cast<ACHPlayerCharacter>(GetOwningActor());
	// if (Character)
	// {
	// 	MovementComponent = Character->GetCharacterMovement();
	// }

	RefreshCachedRefs();
}

void UCHAnimInstance::RefreshCachedRefs()
{
	if (!Character.IsValid())
		Character = Cast<ACHPlayerCharacter>(TryGetPawnOwner());
	if (Character.IsValid() && !MovementComponent.IsValid())
		MovementComponent = Character->GetCharacterMovement();
}

void UCHAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	//
	// if (Character == nullptr || MovementComponent == nullptr)
	// {
	// 	return;
	// }
	//
	// Velocity = MovementComponent->Velocity;
	// GroundSpeed = Velocity.Size2D();
	//
	// bShouldMove = GroundSpeed > 3.f && MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector;
	//
	// bShouldSprint = bShouldMove && GroundSpeed > 600.f;
	//
	// bIsFalling = MovementComponent->IsFalling();

	Super::NativeUpdateAnimation(DeltaSeconds);
	RefreshCachedRefs();
	if (!Character.IsValid() || !MovementComponent.IsValid()) return;

	// Nav 경로추종에서도 일관된 속도 기반 판정
	const FVector Vel      = Character->GetVelocity();
	GroundSpeed            = FVector(Vel.X, Vel.Y, 0.f).Size();
	bIsFalling             = MovementComponent->IsFalling();

	// 1) 이동 여부: 가속도 체크 제거(경로 감속/정지 직전 깜빡임 방지)
	if (!bIsFalling)
	{
		// 시작/종료 임계값 다르게
		if (!bIsMovingCached && GroundSpeed >= WalkStart) bIsMovingCached = true;
		else if (bIsMovingCached && GroundSpeed <= WalkStop) bIsMovingCached = false;
	}
	else
	{
		bIsMovingCached = false; // 공중이면 지상 이동 false
	}
	bShouldMove = bIsMovingCached;

	// 2) 스프린트 여부
	if (bShouldMove && !bIsFalling)
	{
		if (!bIsSprintingCached && GroundSpeed >= SprintOn) bIsSprintingCached = true;
		else if (bIsSprintingCached && GroundSpeed <= SprintOff) bIsSprintingCached = false;
	}
	else
	{
		bIsSprintingCached = false;
	}
	bShouldSprint = bIsSprintingCached;
}

void UCHAnimInstance::UpdateIdle()
{
	CurrentWeapon = Character->GetCurrentWeaponType();
}

void UCHAnimInstance::UpdateAttackMode()
{
	bBeReadyToAttack = Character->GetCurrentCharacterFightMode();
}
