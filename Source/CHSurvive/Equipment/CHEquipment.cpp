// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/CHEquipment.h"

#include "Character/CHPlayerCharacter.h"

// Sets default values
ACHEquipment::ACHEquipment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EquipmentMesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	
	bReplicates = true;
}

// Called when the game starts or when spawned
void ACHEquipment::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACHEquipment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACHEquipment::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (MeshAsset)
	{
		Mesh->SetStaticMesh(MeshAsset);
	}
}

void ACHEquipment::ServerRPCEquipItem()
{
}

void ACHEquipment::EquipItem()
{
}

void ACHEquipment::UnequipItem()
{
}

void ACHEquipment::AttachToOwner(FName SocketName)
{
	if (HasAuthority())
	{
		// 서버면 그냥 직접 실행
		ServerRPCAttachToOwner_Implementation(SocketName);
	}
	else
	{
		// 클라이언트면 서버 RPC 호출
		ServerRPCAttachToOwner(SocketName);
	}
}

void ACHEquipment::ServerRPCAttachToOwner_Implementation(FName SocketName)
{
	// GEngine->AddOnScreenDebugMessage(
	// 				-1, // Key (고유 ID, -1이면 자동으로 갱신됨)
	// 					5.0f, // Duration (화면에 표시될 시간, 초 단위)
	// 						FColor::Green, // 텍스트 색상
	// 							TEXT("ServerRPCAttachToOwner_Implementation") // 출력할 메시지
	// 						);
	ACHPlayerCharacter* PlayerCharacter = Cast<ACHPlayerCharacter>(GetOwner());
	if (PlayerCharacter)
	{
		USkeletalMeshComponent* CharacterMesh = PlayerCharacter->GetMesh();
		if (CharacterMesh)
		{
			AttachToComponent(CharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), SocketName);
		}
	}
}

