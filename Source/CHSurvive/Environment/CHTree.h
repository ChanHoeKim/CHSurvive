// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CHTree.generated.h"

class UBoxComponent;

UCLASS()
class CHSURVIVE_API ACHTree : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACHTree();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, Category="Tree")
	TObjectPtr<UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, Category="Tree")
	TObjectPtr<UStaticMeshComponent> Mesh;
};
