// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_AttackEnd.h"

#include "Character/CHPlayerCharacter.h"

void UAnimNotify_AttackEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (MeshComp)
	{
		ACHPlayerCharacter* CharacterPlayer = Cast<ACHPlayerCharacter>(MeshComp->GetOwner());
		if (CharacterPlayer)
		{
			CharacterPlayer->bCanAttack = true;
		}
	}
}
