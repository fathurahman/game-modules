// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "GUIPlayerController.h"
#include "GUIStatics.generated.h"

UCLASS()
class GAMEUI_API UGUIStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="GUI", meta=(WorldContext="WorldContextObject"))
	static AGUIPlayerController* GetGUIPlayerController(UObject* WorldContextObject, int32 PlayerIndex = 0);

};