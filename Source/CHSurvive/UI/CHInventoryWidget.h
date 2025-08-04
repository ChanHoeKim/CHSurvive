// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CHInventoryWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class CHSURVIVE_API UCHInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<UImage> Slo2t1;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Image")
	TObjectPtr<UTexture2D> LogImage;

	void GetLog();
};
