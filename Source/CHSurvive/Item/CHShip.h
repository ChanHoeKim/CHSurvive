// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Actor.h"
#include "Interface/CHInteractInterface.h"
#include "CHShip.generated.h"

class UInputAction;
class UInputMappingContext;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class CHSURVIVE_API ACHShip : public AActor, public ICHInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACHShip();

protected:
    virtual void BeginPlay() override;

public:
    // 인터랙트 (탑승)
    virtual void Interact(AActor* Interactor) override;

protected:
	UPROPERTY(VisibleAnywhere, Category="Tree")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, Category="Teleport To")
	FName TeleportName;
};
