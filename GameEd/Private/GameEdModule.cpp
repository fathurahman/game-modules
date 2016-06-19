// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GameEdPrivatePCH.h"
#include "AssetToolsModule.h"

DEFINE_LOG_CATEGORY( LogGameObjectEd )

uint32 GameAssetCategory;

class FGameObjectEd : public IGameObjectEd
{
	virtual void StartupModule() override
	{
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		IAssetTools& AssetTools = AssetToolsModule.Get();
		GameAssetCategory = (uint32)AssetTools.RegisterAdvancedAssetCategory("Game", LOCTEXT("Game", "Game"));
	}

	virtual void ShutdownModule() override
	{
		
	}
};

IMPLEMENT_GAME_MODULE(FGameObjectEd, GameObjectEd)