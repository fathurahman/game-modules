// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GamePrivatePCH.h"
#include "Framework/GameManager.h"
#include "Util/GameUtil.h"

TWeakObjectPtr<UGameManager> g_Instance;

UGameManager* UGameManager::Get()
{
	return g_Instance.Get();
}

UGameManager* UGameManager::GetChecked()
{
	check(g_Instance.IsValid());
	return g_Instance.Get();
}

UGameManager::UGameManager( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	SaveGameClass = USaveGameObject::StaticClass();
}

UGame* UGameManager::GetCurrentGame() const
{
	return CurrentGame;
}

UGame* UGameManager::StartGame()
{
	StopGame();

	UClass* Class = GameClass.ResolveClass();
	if ( !Class)
	{
		PrintLogError("GameClass is not valid");
		return nullptr;
	}

	CurrentGame = NewObject<UGame>( Class );
	CurrentGame->Start();

	return CurrentGame;
}

void UGameManager::StopGame()
{
	if ( CurrentGame )
	{
		CurrentGame->Shutdown();
		CurrentGame = nullptr;
	}
}

bool UGameManager::LoadGame( const FString& SlotName, int32 UserIndex )
{
	USaveGameObject* SaveGameObject = Cast<USaveGameObject>( UGameplayStatics::LoadGameFromSlot( SlotName, UserIndex ) );

	if ( !SaveGameObject )
	{
		return false;
	}

	StopGame();

	UClass* Class = GameClass.ResolveClass();
	if ( !Class)
	{
		PrintLogError("GameClass is not valid");
		return nullptr;
	}
	CurrentGame = NewObject<UGame>( Class );
	if ( CurrentGame->LoadFromRecord( SaveGameObject->GameRecord ) == false )
	{
		StopGame();
		return false;
	}

	// TODO: Load world

	PostLoadGame( SaveGameObject );
	BPF_PostLoadGame( SaveGameObject );

	return true;
}

bool UGameManager::SaveGame( const FString& SlotName, int32 UserIndex )
{
	if ( !CurrentGame )
	{
		return false;
	}

	USaveGameObject* SaveGameObject = Cast<USaveGameObject>( UGameplayStatics::CreateSaveGameObject( SaveGameClass.ResolveClass() ) );
	check( SaveGameObject );

	PreSaveGame( SaveGameObject );
	BPF_PreSaveGame( SaveGameObject );

	// TODO: Save world

	if ( CurrentGame->SaveToRecord( SaveGameObject->GameRecord ) == false )
	{
		return false;
	}

	return UGameplayStatics::SaveGameToSlot( SaveGameObject, SlotName, UserIndex );
}

USoundBank* UGameManager::GetSoundBank( const FString& ID ) const
{
	const TAssetPtr<USoundBank> *pAssetPtr = SoundBanks.Find( ID );
	if ( !pAssetPtr )
	{
		return nullptr;
	}
	return GetOrLoadAssetFromPtr( *pAssetPtr );
}

void UGameManager::GetBlueprintGeneratedClassReferences( UClass* ParentClass, TArray<FStringClassReference>& OutClassReferences ) const
{
	// TODO: implement
}

void UGameManager::GetBlueprintGeneratedClasses( UClass* ParentClass, TArray<UClass*>& OutClasses ) const
{
	// TODO: implement
}

void UGameManager::Init()
{
	g_Instance = this;
	Super::Init();

	TArray<TAssetPtr<USoundBank>> SoundBankAssetPtrList;
	GetAssetPtrs( SoundBankAssetPtrList );

	SoundBanks.Empty();
	for ( TAssetPtr<USoundBank> AssetPtr : SoundBankAssetPtrList )
	{
		USoundBank* Bank = GetOrLoadAssetFromPtr( AssetPtr );
		Bank->SetGameInstance( this );
		SoundBanks.Add( Bank->GetID(), AssetPtr );
	}
}

void UGameManager::Shutdown()
{
	Super::Shutdown();
	g_Instance.Reset();
}