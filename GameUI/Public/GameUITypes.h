// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "GameUITypes.generated.h"

UENUM(BlueprintType)
enum class EGUIInputMethod : uint8
{
	Mouse,
	Keyboard,
	Gamepad
};

UENUM(BlueprintType)
enum class EGUIInputKeyPhase : uint8
{
	Capture,
	Target,
	Bubbling
};