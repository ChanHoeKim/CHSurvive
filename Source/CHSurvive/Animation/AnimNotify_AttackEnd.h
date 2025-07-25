// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AttackEnd.generated.h"

/**
 * 
 */
UCLASS(DisplayName="AttackEnd")
class CHSURVIVE_API UAnimNotify_AttackEnd : public UAnimNotify
{
	GENERATED_BODY()

protected:
	virtual  void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
