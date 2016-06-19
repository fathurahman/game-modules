// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GameUIEdPrivate.h"

class FGameUIEdModule : public IGameUIEdModule
{
public:
	virtual void StartupModule() override
	{
	}	

	virtual void ShutdownModule() override
	{
	}

	virtual bool IsGameModule() const override
	{
		return true;
	}
};

IMPLEMENT_GAME_MODULE(FGameUIEdModule, GameUIEd)