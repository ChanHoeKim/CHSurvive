// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/CHInventoryItemImage.h"
#include "Engine/GameInstance.h"
#include "CHGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class CHSURVIVE_API UCHGameInstance : public UGameInstance
{
	GENERATED_BODY()


public:
	UCHGameInstance();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Invetory Item Image")
	TArray<FCHInventoryItemImage> ItemImageTable;
};
