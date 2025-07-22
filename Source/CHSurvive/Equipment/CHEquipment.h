// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CHEquipment.generated.h"

UCLASS()
class CHSURVIVE_API ACHEquipment : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="Equipment | Mesh")
	TObjectPtr<UStaticMesh> MeshAsset;
	
	UPROPERTY(VisibleAnywhere, Category="Equipment | Mesh")
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	
public:	
	// Sets default values for this actor's properties
	ACHEquipment();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual  void OnConstruction(const FTransform& Transform) override;

public:
	virtual void ServerRPCEquipItem();
	virtual void EquipItem();
	
	virtual void UnequipItem();
	virtual void AttachToOwner(FName SocketName);
	UFUNCTION(Server, Reliable)
	void ServerRPCAttachToOwner(FName SocketName);
	
};
