// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CHInventoryWidget.h"

#include "Components/Image.h"
#include "Controller/CHPlayerController.h"
#include "Data/CHInventoryItemImage.h"
#include "Environment/CHLog.h"
#include "Game/CHGameInstance.h"

void UCHInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Slot1)
	{
		Slot1->SetColorAndOpacity(FLinearColor(0.003347, 0.002428, 0.002428));
	}
}

void UCHInventoryWidget::GetLog(ACHLog* InLog)
{
	if (Slot1)
	{
		Slot1->SetColorAndOpacity(FColor::White);
		Slot1->SetBrushFromTexture(InLog->GetLogIcon());
	}
}
