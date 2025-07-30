// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/CHLog.h"

#include "Components/CapsuleComponent.h"

// Sets default values
ACHLog::ACHLog()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	
	
	bReplicates = true;

	LogAsset = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LogAsset"));
	RootComponent = LogAsset;

	// 충돌 및 물리 기본 설정
	LogAsset->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	LogAsset->SetCollisionObjectType(ECC_PhysicsBody);
	LogAsset->SetCollisionResponseToAllChannels(ECR_Block);
	LogAsset->SetSimulatePhysics(true);
	LogAsset->SetEnableGravity(true);
}

// Called when the game starts or when spawned
void ACHLog::BeginPlay()
{
	Super::BeginPlay();

	if (LogMesh)
	{
		LogAsset->SetStaticMesh(LogMesh);
	}
}

void ACHLog::Interact(AActor* Interactor)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
				-1, // Key (고유 ID, -1이면 자동으로 갱신됨)
					5.0f, // Duration (화면에 표시될 시간, 초 단위)
						FColor::Green, // 텍스트 색상
							TEXT("통나무 드랍") // 출력할 메시지
							);
	}
}

// Called every frame
void ACHLog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// void ACHLog::OnConstruction(const FTransform& Transform)
// {
// 	Super::OnConstruction(Transform);
// 	if (EquipmentClass)
// 	{
// 		if (AActor* CDO = EquipmentClass->GetDefaultObject<AActor>())
// 		{
// 			Mesh->SetStaticMesh(CDO->MeshAsset);
// 			Mesh->SetSimulatePhysics(true);
// 		}
// 	}
// }

