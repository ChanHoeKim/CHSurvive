// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CHCombatComponent.h"

#include "Character/CHPlayerCharacter.h"
#include "Equipment/CHWeapon.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UCHCombatComponent::UCHCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	//bReplicates = true;
	// ...
	//SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UCHCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UCHCombatComponent::OnRep_EquippedWeapon()
{
	GEngine->AddOnScreenDebugMessage(
					-1, // Key (고유 ID, -1이면 자동으로 갱신됨)
						5.0f, // Duration (화면에 표시될 시간, 초 단위)
							FColor::Green, // 텍스트 색상
								TEXT("OnRep_EquippedWeapon") // 출력할 메시지
							);
	// ACHWeapon* Weapon = Cast<ACHWeapon>(MainWeapon);
	// if (Weapon)
	// {
	// 	Weapon->EquipItem();
	// }
}

void UCHCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MainWeapon);
}


// Called every frame
void UCHCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCHCombatComponent::SetWeapon(ACHWeapon* NewWeapon)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		MainWeapon = NewWeapon;
	}
	// GEngine->AddOnScreenDebugMessage(
	// 				-1, // Key (고유 ID, -1이면 자동으로 갱신됨)
	// 					5.0f, // Duration (화면에 표시될 시간, 초 단위)
	// 						FColor::Green, // 텍스트 색상
	// 							TEXT("SetWeapon") // 출력할 메시지
	// 						);
}

