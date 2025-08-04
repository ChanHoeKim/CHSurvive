// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CHInventoryWidget.h"

#include "Components/Image.h"
#include "Controller/CHPlayerController.h"
#include "Data/CHInventoryItemImage.h"
#include "Game/CHGameInstance.h"

void UCHInventoryWidget::GetLog()
{
	
	Slot1->SetColorAndOpacity(FColor::White);
	ACHPlayerController* PC = Cast<ACHPlayerController>(GetOwningPlayer());
	if (PC)
	{
		UCHGameInstance* GameInstance = Cast<UCHGameInstance>(PC->GetGameInstance());
		if (GameInstance)
		{
			for (const FCHInventoryItemImage& Table : GameInstance->ItemImageTable)
			{
				if (Table.ItemName == FName("Log"))
				{
					Slot1->SetBrushFromTexture(Table.Icon);
				}
			}
		}
	}
}
