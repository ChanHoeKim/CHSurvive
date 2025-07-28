// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CHDefine.h"
#include "GameFramework/Character.h"
#include "CHPlayerCharacter.generated.h"

class UCHCombatComponent;
/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;

UCLASS(Blueprintable)
class ACHPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACHPlayerCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;


private:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* ClickAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* ReadyToAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* AttackAction;


protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	void Sprint();
	void SprintEnd();
	void ReadyToAttackEnd();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void OnClickMove();
	void CheckInteract();

	void Interact();
	UFUNCTION(Server, Reliable)
	void ServerInteract();

	void ReadyToAttack();

	
	

	UFUNCTION(Server, Reliable)
	void ServerRPCAttack();

	void AttackAnimationPlay();

	UFUNCTION(Client, Reliable)
	void ClientRPCPlayAnimation(ACHPlayerCharacter* CharacterType);
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCHCombatComponent> CombatComponent;
	
public:
	void Attack();

	void AttackHitCheck();
	
	uint8 bTargetAttack:1 = false;
	FTimerHandle TargetAttackTimerHandle;

	void OnOffTagetAutoAttack(bool InOnOff);
	//void TagetAutoAttackEnd();
	
	UPROPERTY(ReplicatedUsing=OnRep_ChangebBeReadyToAttack)
	uint8 bBeReadyToAttack:1 = false;

	UFUNCTION(Server, Reliable)
	void ServerRPCChangebBeReadyToAttack(bool InBool);

	UFUNCTION(Server, Reliable)
	void ServerRPCAttackTargetEnd();
	
	uint8 bShouldMove:1 = false;

	uint8 bCanAttack:1=true;
	
	float MoveTargetAllowRadius = 10.0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WalkSpeed")
	float CharacterMaxWalkSpeed = 600;

	UFUNCTION(Server, Reliable)
	void ServerRPCChangeCharacterMaxWalkSpeed(float Speed);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character Target Point", meta=(AllowPrivateAccess))
	FVector TargetPoint;


private:
	UFUNCTION()
	void OnRep_ChangebBeReadyToAttack();
	
	UPROPERTY(ReplicatedUsing=OnRep_ChangeWeaponType)
	EWeaponType CurrentWeapon = EWeaponType::None;

	UFUNCTION()
	void OnRep_ChangeWeaponType();


public:
	FORCEINLINE EWeaponType GetCurrentWeaponType() {return CurrentWeapon;}
	FORCEINLINE bool GetCurrentCharacterFightMode() {return bBeReadyToAttack;}
	
	void ChangeWeaponType(EWeaponType WeaponType);
};

