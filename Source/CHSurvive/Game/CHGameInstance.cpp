// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/CHGameInstance.h"

UCHGameInstance::UCHGameInstance()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
				-1, // Key (고유 ID, -1이면 자동으로 갱신됨)
					5.0f, // Duration (화면에 표시될 시간, 초 단위)
						FColor::Green, // 텍스트 색상
							TEXT("UCHGameInstance") // 출력할 메시지
							);
	}
}
