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
	TWeakObjectPtr<ACHPlayerCharacter> Character;

	UPROPERTY(EditAnywhere, Category="Reference")
	TWeakObjectPtr<UCharacterMovementComponent> MovementComponent;
	
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
	void RefreshCachedRefs();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	void UpdateIdle();
	void UpdateAttackMode();



	// Nav 이동에 맞춘 임계값 (원하면 값만 조절)
	static constexpr float WalkStart =  60.f;  // 걷기 시작(감속 구간에서도 안정)
	static constexpr float WalkStop  =  30.f;  // 걷기 종료(히스테리시스 하한)
	static constexpr float SprintOn  = 600.f;  // 스프린트 시작
	static constexpr float SprintOff = 540.f;  // 스프린트 종료(깜빡임 방지)

	// 상태 유지용 내부 캐시
	bool bIsMovingCached   = false;
	bool bIsSprintingCached= false;







};
