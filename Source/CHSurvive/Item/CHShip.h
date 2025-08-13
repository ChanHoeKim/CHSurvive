// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/CHInteractInterface.h"
#include "CHShip.generated.h"

class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class CHSURVIVE_API ACHShip : public APawn, public ICHInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACHShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mesh")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mesh")
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mesh")
	TObjectPtr<UStaticMeshComponent> Mesh;

	// 헤더
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Placement")
	USceneComponent* AttachPoint;

	UPROPERTY(VisibleAnywhere)
	UFloatingPawnMovement* Movement;
	
	virtual void Interact(AActor* Interactor) override;
};
