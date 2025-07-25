// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CHDefine.h"
#include "Animation/AnimInstance.h"
#include "CHAnimInstance.generated.h"

class UCharacterMovementComponent;
class ACHPlayerCharacter;
/**
 * 
 */
UCLASS()
class CHSURVIVE_API UCHAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category="Reference")
	TObjectPtr<ACHPlayerCharacter> Character;

	UPROPERTY(EditAnywhere, Category="Reference")
	TObjectPtr<UCharacterMovementComponent> MovementComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType CurrentWeapon = EWeaponType::None; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bBeReadyToAttack:1=false;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Data")
	FVector Velocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Data")
	float GroundSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Data")
	uint8 bShouldMove:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Data")
	uint8 bShouldSprint:1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Data")
	uint8 bIsFalling:1;

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	void UpdateIdle();
	void UpdateAttackMode();
};
