// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GamePrivatePCH.h"
#include "AssetRegistryModule.h"
#include "GameStatics.h"
#include "Util/GameUtil.h"
#include "Components/GameTagsComponent.h"
#include "Framework/SoundBank.h"
#include "Framework/GameManager.h"

FRandomStream GRandomStream;


UGameManager* UGameStatics::GetGameManager()
{
	return UGameManager::Get();
}

UGame* UGameStatics::GetGame()
{
	return UGame::Get();
}

UGame* UGameStatics::GetGameSafe()
{
	return UGame::GetSafe();
}

void UGameStatics::GetObjectsWithTag( FString InTag, TSubclassOf<UObjectWithTags> Class, TArray<UObjectWithTags*>& OutObjects )
{
	OutObjects.Empty();	
	for ( TObjectIterator<UObjectWithTags> It; It; ++It )
	{
		UObjectWithTags* Obj = *It;
		if ( !Obj->HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject) && Obj->IsA( Class ) && Obj->HasTag(InTag) )
		{
			OutObjects.Add( Obj );
		}
	}
}


void UGameStatics::GetObjectsWithTags(ETagFilterType FilterType, TArray<FString> InTags, TSubclassOf<UObjectWithTags> Class, TArray<UObjectWithTags*>& OutObjects)
{
	OutObjects.Empty();	
	for ( TObjectIterator<UObjectWithTags> It; It; ++It )
	{
		UObjectWithTags* Obj = *It;
		if ( !Obj->HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject) && Obj->IsA( Class ) )
		{
			if ( FilterType == ETagFilterType::HasAnyTags )
			{
				if ( Obj->HasAnyTags(InTags) )
				{
					OutObjects.Add( Obj );
				}
			}
			else
			{
				if ( Obj->HasAllTags(InTags) )
				{
					OutObjects.Add( Obj );
				}
			}
		}
	}

}


void UGameStatics::GetActorsWithTag(UObject* WCO, FString InTag, TArray<AActor*>& OutActors)
{
	OutActors.Empty();

	UWorld* World = GEngine->GetWorldFromContextObject( WCO, false );
	if (!World)
	{
		return;
	}

	for ( TActorIterator<AActor> It(World); It; ++It )
	{
		AActor* Actor = *It;
		if ( UGameTagsComponent* Comp = Cast<UGameTagsComponent>( Actor->GetComponentByClass( UGameTagsComponent::StaticClass() ) ) )
		{
			if ( Comp->GetTags()->HasTag( InTag ) )
			{
				OutActors.Add( Actor );
			}
		}
	}
}


void UGameStatics::GetActorsWithTags(UObject* WCO, ETagFilterType FilterType, TArray<FString> InTags, TArray<AActor*>& OutActors)
{
	OutActors.Empty();

	UWorld* World = GEngine->GetWorldFromContextObject( WCO, false );
	if (!World)
	{
		return;
	}

	for ( TActorIterator<AActor> It(World); It; ++It )
	{
		AActor* Actor = *It;
		if ( UGameTagsComponent* Comp = Cast<UGameTagsComponent>( Actor->GetComponentByClass( UGameTagsComponent::StaticClass() ) ) )
		{
			if ( FilterType == ETagFilterType::HasAnyTags && Comp->GetTags()->HasAnyTags( InTags ) )
			{
				OutActors.Add( Actor );
			}
			else if ( FilterType == ETagFilterType::HasAllTags && Comp->GetTags()->HasAllTags( InTags ) )
			{
				OutActors.Add( Actor );
			}
		}
	}
}

void UGameStatics::GetGameDataByClass(TSubclassOf<UGameData> Class, TArray<UGameData*>& OutList)
{
	FAssetRegistryModule&  AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> AssetDataList;
	AssetRegistry.GetAssetsByClass(Class->GetFName(), AssetDataList);
	for ( const FAssetData& AssetData : AssetDataList )
	{
		UObject* Object = AssetData.GetAsset();
		if ( UGameData* GameData = Cast<UGameData>(Object) )
		{
			OutList.Add(GameData);
		}
	}
}

void UGameStatics::GetGameObjectClasses(TSubclassOf<UGameObject> Class, TArray<UClass*>& OutClasses)
{
	for (TObjectIterator<UClass> It; It; ++It)
	{
		if (Class->IsChildOf(*It))
		{
			OutClasses.Add(*It);
		}
	}
}


USoundBank* UGameStatics::GetSoundBank( USoundBank* SoundBank )
{
	return SoundBank;
}

bool UGameStatics::BPF_PlaySoundBankEntry( USoundBank* SoundBank, FString SoundID )
{
	if ( !SoundBank )
	{
		return false;
	}
	return SoundBank->Play( SoundID );
}

bool UGameStatics::BPF_PlaySoundBankEntryAtLocation(USoundBank* SoundBank, FString SoundID, FVector Location, FRotator Rotation)
{
	if ( !SoundBank )
	{
		return false;
	}
	return SoundBank->PlayAtLocation( SoundID, Location, Rotation );
}

UAudioComponent* UGameStatics::BPF_PlaySoundBankEntryAttached(USoundBank* SoundBank, FString SoundID, class USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroyed)
{
	if ( !SoundBank )
	{
		return nullptr;
	}
	return SoundBank->PlayAttached( SoundID, AttachToComponent, AttachPointName, Location, Rotation, LocationType, bStopWhenAttachedToDestroyed );
}

UBlueprintClassRegistry* UGameStatics::BPF_GetBlueprintClassRegistry( UClass* ParentClass )
{
	return GetBlueprintClassRegistry( ParentClass );
}

UTexture2D* UGameStatics::BPF_LoadTexture2DFromFile( const FString& FullFilePath, EImageFileFormat ImageFileFormat, int32& OutWidth, int32& OutHeight, ESuccess& OutResult )
{
	UTexture2D* Texture = LoadTexture2DFromFile( FullFilePath, ImageFileFormat, OutWidth, OutHeight );
	OutResult = Texture ? ESuccess::Success : ESuccess::Fail;
	return Texture;
}

