// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/CHTree.h"

#include "Components/BoxComponent.h"

// Sets default values
ACHTree::ACHTree()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void ACHTree::BeginPlay()
{
	Super::BeginPlay();
	
}



