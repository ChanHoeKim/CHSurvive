// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/CHInteractInterface.h"
#include "CHPickupItem.generated.h"

class ACHEquipment;

UCLASS()
class CHSURVIVE_API ACHPickupItem : public AActor, public ICHInteractInterface
{
	GENERATED_BODY()

protected:
	

public:	
	// Sets default values for this actor's properties
	ACHPickupItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Interact(AActor* Interactor) override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="PickupItem")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PickupItem")
	TSubclassOf<ACHEquipment> EquipmentClass;

};
