// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GameUIPrivatePCH.h"
#include "Kismet/GameplayStatics.h"
#include "GUIStatics.h"

AGUIPlayerController* UGUIStatics::GetGUIPlayerController(UObject* WorldContextObject, int32 PlayerIndex)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex);
	return CastChecked<AGUIPlayerController>(PlayerController);
}

