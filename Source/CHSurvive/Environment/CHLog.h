// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/CHInteractInterface.h"
#include "CHLog.generated.h"

UCLASS()
class CHSURVIVE_API ACHLog : public AActor, public ICHInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACHLog();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Interact(AActor* Interactor) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Asset")
	// TObjectPtr<UStaticMeshComponent> LogAsset;

	// UPROPERTY(VisibleAnywhere, Category="Asset")
	// TObjectPtr<UStaticMeshComponent> LogAsset;
	//
	// UPROPERTY(EditAnywhere, Category="Asset")
	// TObjectPtr<UStaticMesh> LogMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh")
	UStaticMesh* LogMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LogAsset;
};
