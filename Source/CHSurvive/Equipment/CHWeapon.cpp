// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/CHWeapon.h"

#include "Component/CHCombatComponent.h"

// Sets default values
ACHWeapon::ACHWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ACHWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACHWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// void ACHWeapon::ServerRPCEquipItem_Implementation()
// {
// 	// GEngine->AddOnScreenDebugMessage(
// 	// 				-1, // Key (고유 ID, -1이면 자동으로 갱신됨)
// 	// 					5.0f, // Duration (화면에 표시될 시간, 초 단위)
// 	// 						FColor::Green, // 텍스트 색상
// 	// 							TEXT("ServerRPCEquipItem_Implementation") // 출력할 메시지
// 	// 						);
// 	CombatComponent = GetOwner()->GetComponentByClass<UCHCombatComponent>();
//
// 	if (CombatComponent)
// 	{
// 		CombatComponent->SetWeapon(this);
// 		AttachToOwner(EquipSocketName);
// 	}
// }

void ACHWeapon::EquipItem()
{
	Super::EquipItem();

	CombatComponent = GetOwner()->GetComponentByClass<UCHCombatComponent>();

	if (CombatComponent)
	{
		CombatComponent->SetWeapon(this);
		AttachToOwner(EquipSocketName);
	}
}

