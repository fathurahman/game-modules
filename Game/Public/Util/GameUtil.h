#pragma once

#include "GameTypes.h"
#include "CoreUtil.h"
#include "AssetRegistryModule.h"

template<class T=UObjectWithTags>
void GetObjectsWithTag( const FString& InTag, TArray<T*>& OutResult, EObjectFlags FlagsToCheck = RF_NoFlags )
{
	OutResult.Empty();
	for (TObjectIterator<T> It; It; ++It)
	{
		T* Object = *It;
		if ( FlagsToCheck == RF_NoFlags || Object->HasAllFlags( FlagsToCheck ) == true )
		{
			if (Object->HasTag( InTag ))
			{
				OutResult.Add( Object );
			}
		}
	}
}

template<class T=UObjectWithTags>
void GetAssetsWithTag( const FString& InTag, TArray<T*>& OutResult, bool bSeachSubClasses = false )
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetDataList;
	AssetRegistryModule.Get().GetAssetsByClass( T::StaticClass()->GetFName(), AssetDataList, bSeachSubClasses );

	OutResult.Empty();
	for ( const FAssetData& AssetData : AssetDataList )
	{
		T* Object = Cast<T>( AssetData.GetAsset() );
		if ( Object->HasTag( InTag ) )
		{
			OutResult.Add( Object );
		}
	}
}

template<class T=UObject>
void GetAssets( TArray<T*>& OutResult, bool bSearchSubClasses = false )
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	
	TArray<FAssetData> AssetDataList;
	AssetRegistryModule.Get().GetAssetsByClass( T::StaticClass()->GetFName(), AssetDataList, bSearchSubClasses );

	OutResult.Empty();
	for ( const FAssetData& AssetData : AssetDataList )
	{
		T* Object = Cast<T>( AssetData.GetAsset() );
		OutResult.Add( Object );
	}
}

template<class T=UObject>
void GetAssetRefs( TArray<FStringAssetReference>& OutResult, bool bSearchSubClasses = false )
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	
	TArray<FAssetData> AssetDataList;
	AssetRegistryModule.Get().GetAssetsByClass( T::StaticClass()->GetFName(), AssetDataList, bSearchSubClasses );

	OutResult.Empty();
	for ( const FAssetData& AssetData : AssetDataList )
	{
		FStringAssetReference Ref = AssetData.ToStringReference();
		OutResult.Add( Ref );
	}
}

template<class T=UObject>
void GetAssetPtrs( TArray<TAssetPtr<T>>& OutResult, bool bSearchSubClasses = false )
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	
	TArray<FAssetData> AssetDataList;
	AssetRegistryModule.Get().GetAssetsByClass( T::StaticClass()->GetFName(), AssetDataList, bSearchSubClasses );

	OutResult.Empty();
	for ( const FAssetData& AssetData : AssetDataList )
	{
		FStringAssetReference Ref = AssetData.ToStringReference();
		TAssetPtr<T> Ptr;
		Ptr = Ref;
		OutResult.Add( Ptr );
	}
}

template<class T=AActor>
FORCEINLINE const TArray<T*>& GetCachedActors( UWorld* World, TArray<T*>& Cache, bool bRepopulateInEditor = true )
{
	check( World );

	bool bPopulate = true;
	if ( !bRepopulateInEditor || World->GetGameInstance() != nullptr )
	{
		bPopulate = ( Cache.Num() == 0 );
	}

	if ( bPopulate )
	{
		Cache.Empty();
		for ( TActorIterator<T> It(World); It; ++It)
		{
			T* Actor = *It;
			Cache.Add( Actor );
		}
	}

	return Cache;
}

class UBlueprintClassRegistry;

GAME_API UBlueprintClassRegistry* GetBlueprintClassRegistry( UClass* ParentClass );

GAME_API void GetClasses( UClass* ClassToSearch, TArray<UClass*>& OutResult, bool bAllowAbstract = false, bool bBlueprintClassOnly = false  );

template<class T=UGameObject>
FORCEINLINE void GetClasses( TArray<UClass*>& OutResult, bool bAllowAbstract = false, bool bBlueprintClassOnly = false )
{
	GetClasses( T::StaticClass(), OutResult, bAllowAbstract, bBlueprintClassOnly );
}



/** Get a random index of an array of random weighted struct. 
* @param	Array			Array of random weighter struct.
* @param	RandomStream	(optional) Whether to use a certain random stream to get the randomize index.
* @return INDEX_NONE if a random index can not be generated.
*/
template<typename T=FRandomWeightedStruct>
int32 GetRandomWeightedIndex(const TArray<T>& Array, FRandomStream* RandomStream = nullptr)
{
	if ( Array.Num() == 0 )
	{
		return INDEX_NONE;
	}

	int32 Index;
	int32 TotalWeight = 0;
	for ( Index = 0; Index < Array.Num(); Index++)
	{
		const T& Entry = Array[Index];
		TotalWeight += Entry.RandomWeight;
	}

	int32 ChosenWeight = RandomStream ? RandomStream->RandHelper(TotalWeight) : FMath::RandHelper(TotalWeight);

	for ( Index = 0; Index < Array.Num(); Index++ )
	{
		const T& Entry = Array[Index];
		if ( ChosenWeight < Entry.RandomWeight )
		{
			return Index;
		}
		ChosenWeight -= Entry.RandomWeight;
	}

	return INDEX_NONE;
}

GAME_API class UGameObject* StaticCreateGameObject( TSubclassOf<UGameObject> Class );

template<class T=UGameObject>
FORCEINLINE T* CreateGameObject()
{
	return Cast<T>( StaticCreateGameObject( T::StaticClass() ) );
}

template<class T=UGameObject>
FORCEINLINE T* CreateGameObject( TSubclassOf<T> Class )
{
	return Cast<T>( StaticCreateGameObject( Class ) );
}

template<typename T=FNamedData, typename ValueType>
FORCEINLINE ValueType GetNamedDataValue( const TArray<T>& List, const FString& Name, const ValueType& DefaultValue )
{
	if ( Name.IsEmpty() )
	{
		return DefaultValue;
	}

	for ( const T& Entry : List )
	{
		if ( Entry.Name == Name )
		{
			return Entry.Value;
		}
	}
	return DefaultValue;
}

/**
* Set a named data value in a list.
* @param	List	Named data list.
* @param	Name	Name of the data which value is about to be changed.
* @param	Value	The new value.
* @return true if the value actually changed.
*/
template<typename T=FNamedData, typename ValueType>
FORCEINLINE bool SetNamedDataValue( TArray<T>& List, const FString& Name, const ValueType& Value )
{
	if ( Name.IsEmpty() )
	{
		return false;
	}
	for ( T& Entry : List )
	{
		if ( Entry.Name == Name )
		{
			if ( Entry.Value == Value )
			{
				return false;
			}
			else
			{
				Entry.Value = Value;
				return true;
			}
		}
	}

	T NewEntry;
	NewEntry.Name = Name;
	NewEntry.Value = Value;
	List.Add( NewEntry );
	return true;
}

FORCEINLINE ESuccess GetSuccessEnum( bool bFlag )
{
	return bFlag ? ESuccess::Success : ESuccess::Fail;
}

FORCEINLINE EFindResult GetFindResultEnum( bool bFlag )
{
	return bFlag ? EFindResult::Found : EFindResult::NotFound;
}

FORCEINLINE EValidity GetValidityEnum( bool bFlag )
{
	return bFlag ? EValidity::Valid : EValidity::NotValid;	
}

FORCEINLINE EValidity GetValidityEnum( UObject* Object )
{
	return GetValidityEnum( Object != nullptr && Object->IsPendingKillOrUnreachable() == false );
}


//////////////////////////////////////////////////////////////////////////
// Sound Effect
//////////////////////////////////////////////////////////////////////////

/**
* Play a 2D sound bank's entry. (Not Replicable)
* This is Fire and forget playback, you play the sound and you just forget about it. The audio component will destroy itself when it's finished.
* @param	SoundBankID		The sound bank's ID.
* @param	SoundID			The sound bank entry's ID.
* @param	World			(Optional) The world where the sound will be played in, if null then the current world is used instead.
* @return false if the sound can not be played.
*/
GAME_API bool PlaySoundBankEntry( const FString& SoundBankID, const FString& SoundID, UWorld* World = nullptr );

/**
* Play a sound bank entry at a certain location in the world. (Replicable)
* @param	SoundBankID		The sound bank's ID.
* @param	SoundID			The sound bank entry's ID.
* @param	Location		The location in which the sound will be played in.
* @param	Rotation		The sound's rotation
* @param	World			(Optional) THe world where the sound will be played in, if null then the current world is used instead.
* @return false if the sound can not be played.
*/
GAME_API bool PlaySoundBankEntryAtLocation( const FString& SoundBankID, const FString& SoundID, const FVector& Location, const FRotator& Rotation = FRotator::ZeroRotator, UWorld* World = nullptr );

/**
* Play a sound bank entry that is attached to a certain scene component.
* @param	SoundBankID						The sound bank's ID.
* @param	SoundID							The sound bank entry's ID.
* @param	AttachToComponent				The component where the sound will be attached to
* @param	AttachPointName					The slot where the sound will be attached to
* @param	Location						Attachment location
* @param	Rotation						Attachment rotation
* @param	LocationType					Attachment location's type
* @param	bStopWhenAttachedToDestroyed	Whether to stop the sound when the component it is attached to is destroyed.
* @return nullptr if the attachment failed.
*/
GAME_API UAudioComponent* PlaySoundBankEntryAttached( const FString& SoundBankID, const FString& SoundID, class USceneComponent* AttachToComponent, const FName& AttachPointName = NAME_None, const FVector& Location = FVector::ZeroVector, const FRotator& Rotation = FRotator::ZeroRotator, EAttachLocation::Type LocationType = EAttachLocation::KeepRelativeOffset, bool bStopWhenAttachedToDestroyed = false);

//////////////////////////////////////////////////////////////////////////
// Texture and Material
//////////////////////////////////////////////////////////////////////////

GAME_API UTexture2D* LoadTexture2DFromFile( const FString& FilePath, EImageFileFormat ImageFileFormat, int32& OutWidth, int32& OutHeight );

//////////////////////////////////////////////////////////////////////////
// Debug Utilities
//////////////////////////////////////////////////////////////////////////

/** Convert object flags to string. */
GAME_API FString ObjectFlagsToString( EObjectFlags ObjectFlags );

/** Convert class flags to string. */
GAME_API FString ClassFlagsToString( uint32 ClassFlags );

/** Convert property flags to string. */
GAME_API FString PropertyFlagsToString( uint64 PropertyFlags );

// Flags for TraceObject
#define TOF_OBJECT_DETAILS		0x01
#define TOF_CLASS_DETAILS		0x02
#define TOF_PROPERTY_DETAILS	0x08

#define TOF_ALL_DETAILS			0xff
#define TOF_NONE				0x00
// End of flags for TraceObject

/**
* Print information of an object to the log.
* @note: This function only works in debug builds. (DebugGame or DebugGameEditor), it does nothing in Development or Shipping build.
* @param	Object					The object whose information will be written to the log.
* @param	Title					The title that will be added at the beginning of the information.
* @param	PropertyNameToTrace		List of property names to be included in the the information.
* @param	TraceObjectFlags		Bit flags of what details to be added to the information, the default value is to print all details.
*/
GAME_API void TraceObject( const UObject* Object, const FString& Title = TEXT(""), const TArray<FName>& PropertyNamesToTrace = TArray<FName>(), int32 TraceObjectFlags = TOF_ALL_DETAILS  );

//////////////////////////////////////////////////////////////////////////
// World Related
//////////////////////////////////////////////////////////////////////////

template<class T=AActor>
FORCEINLINE T* FindActor(UWorld* World)
{
	for ( TActorIterator<T> It(World); It; ++It )
	{
		return *It;
	}
	return nullptr;
}

template<class T=AActor>
FORCEINLINE bool FindActors(UWorld* World, TArray<T*>& OutActors)
{
	OutActors.Empty();
	for ( TActorIterator<T> It(World); It; ++It )
	{
		OutActors.Add( *It );
	}
	return OutActors.Num() > 0;
}

