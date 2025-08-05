// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Environment/CHLog.h"
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
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<UImage> Slot1;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Image")
	TObjectPtr<UTexture2D> LogImage;

	void GetLog(ACHLog* InLog);
};
