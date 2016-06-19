// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "Engine.h"

#define LOCTEXT_NAMESPACE "GameModule"

DECLARE_LOG_CATEGORY_EXTERN( LogGame, Log, All );
DECLARE_LOG_CATEGORY_EXTERN( LogTrace, Log, All );

#define PRINT_LOG_CATEGORY LogGame
#define TRACE_LOG_CATEGORY LogTrace

#include "CoreUtil.h"

// Just a little something to remove red squiggles in visual studio
#ifndef GAME_API
#define GAME_API
#endif