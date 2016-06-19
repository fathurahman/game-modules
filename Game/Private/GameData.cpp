// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GamePrivatePCH.h"
#include "Framework/Game.h"

UGameObject* UGameData::GetInstance()
{
	if ( InstanceClass == nullptr )
	{
		PrintLogWarning( "Trying to get an instance of a game data that has not instance class: %s", *GetFullName() );
		return nullptr;
	}

	if ( Instance.IsValid() == false )
	{
		if ( UGame* Game = UGame::Get() )
		{
			FString InstanceID;
			GetPathName( nullptr, InstanceID );

			UGameObjectContainer* Container = Game->GetObjectTree()->GetOrCreateContainerForObject( UGameObject::StaticClass() );

			Instance = Container->GetOrCreateChild<UGameObject>( InstanceClass, InstanceID );
			OnInitInstance();
			UGameObject* Object = Instance.Get();
			BPF_OnInitInstance( Object );
		}
	}

	return Instance.Get();
}
