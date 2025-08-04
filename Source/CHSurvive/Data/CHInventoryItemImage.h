// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CHInventoryItemImage.generated.h"

class UImage;
/**
 * 
 */
USTRUCT(BlueprintType)
struct CHSURVIVE_API FCHInventoryItemImage : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	FCHInventoryItemImage();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Image")
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Image")
	UTexture2D* Icon;
};
