// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CHEquipment.h"
#include "GameFramework/Actor.h"
#include "CHWeapon.generated.h"

class UCHCombatComponent;

UCLASS()
class CHSURVIVE_API ACHWeapon : public ACHEquipment
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Socket Name")
	FName EquipSocketName;
	
public:	
	// Sets default values for this actor's properties
	ACHWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//CombatComponent의 무기를 가져오기
	UPROPERTY()
	TObjectPtr<UCHCombatComponent> CombatComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack Animation")
	TObjectPtr<UAnimMontage> AttackAnim;

public:
	UPROPERTY(EditAnywhere)
	FName TraceStartSocketName;

	UPROPERTY(EditAnywhere)
	FName TraceEndSocketName;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// UFUNCTION(Server, Reliable)
	// virtual void ServerRPCEquipItem() override;
	virtual void EquipItem() override;
	FORCEINLINE UAnimMontage* GetAttackAnim(){return AttackAnim;}
};
