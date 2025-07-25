// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CHAnimInstance.h"

#include "Character/CHPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


void UCHAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ACHPlayerCharacter>(GetOwningActor());
	if (Character)
	{
		MovementComponent = Character->GetCharacterMovement();
	}
}

void UCHAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Character == nullptr || MovementComponent == nullptr)
	{
		return;
	}

	Velocity = MovementComponent->Velocity;
	GroundSpeed = Velocity.Size2D();

	bShouldMove = GroundSpeed > 3.f && MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector;

	bShouldSprint = bShouldMove && GroundSpeed > 600.f;
	
	bIsFalling = MovementComponent->IsFalling();
}

void UCHAnimInstance::UpdateIdle()
{
	CurrentWeapon = Character->GetCurrentWeaponType();
}

void UCHAnimInstance::UpdateAttackMode()
{
	bBeReadyToAttack = Character->GetCurrentCharacterFightMode();
}
