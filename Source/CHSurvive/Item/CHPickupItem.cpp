// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/CHPickupItem.h"

#include "Equipment/CHEquipment.h"

// Sets default values
ACHPickupItem::ACHPickupItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Item Mesh"));
	SetRootComponent(Mesh);
	bReplicates = true;
}

// Called when the game starts or when spawned
void ACHPickupItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACHPickupItem::Interact(AActor* Interactor)
{
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(
	// 			-1, // Key (고유 ID, -1이면 자동으로 갱신됨)
	// 				5.0f, // Duration (화면에 표시될 시간, 초 단위)
	// 					FColor::Green, // 텍스트 색상
	// 						TEXT("상자에 intraction") // 출력할 메시지
	// 					);
	// }
	if (!HasAuthority()) // 서버에서만 처리
	{
		// 클라이언트라면 서버에 RPC 호출하는 로직 필요
		return;
	}
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Interactor;

	ACHEquipment* SpawnItem = GetWorld()->SpawnActor<ACHEquipment>(EquipmentClass, GetActorTransform(), SpawnParams);
	if (SpawnItem)
	{
		SpawnItem->EquipItem();
		Destroy();
	}
}

void ACHPickupItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (EquipmentClass)
	{
		if (ACHEquipment* CDO = EquipmentClass->GetDefaultObject<ACHEquipment>())
		{
			Mesh->SetStaticMesh(CDO->MeshAsset);
		}
	}
}

// Called every frame
void ACHPickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}