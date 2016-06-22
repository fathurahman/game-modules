#include "GamePrivatePCH.h"

#include "Object/GameData.h"
#include "GameUtil.h"
#include "AssetRegistryModule.h"
#include "GameDataManager.h"

UGameDataManager* UGameDataManager::Get()
{
	return CastChecked<UGameDataManager>( GEngine->GameSingleton );
}

UGameDataManager::UGameDataManager( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{

}

USoundBank* UGameDataManager::GetSoundBank( const FString& ID ) const
{
	if ( SoundBankAssetPtrMap.Num() == 0 )
	{
		for ( TAssetPtr<USoundBank> AssetPtr : SoundBankAssetPtrs )
		{
			USoundBank* SoundBank = GetOrLoadAssetFromPtr( AssetPtr );
			FString ID = SoundBank->GetID();
			SoundBankAssetPtrMap.Add( ID, AssetPtr );
		}
	}

	if ( const TAssetPtr<USoundBank>* pAssetPtr = SoundBankAssetPtrMap.Find(ID) )
	{
		return GetOrLoadAssetFromPtr( *pAssetPtr );
	}

	return nullptr;
}

void UGameDataManager::PostInitProperties()
{
	Super::PostInitProperties();

	if ( GetOutermost() == GetTransientPackage() && !HasAnyFlags(RF_ArchetypeObject | RF_ClassDefaultObject) )
	{
#if WITH_EDITOR
		FAssetRegistryModule&  AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
		AssetRegistry.OnInMemoryAssetCreated().AddUObject( this, &UGameDataManager::OnInMemoryAssetCreated );
		AssetRegistry.OnInMemoryAssetDeleted().AddUObject( this, &UGameDataManager::OnInMemoryAssetDeleted );
#else
		GameDataAssetReferences.Empty();
#endif
	}
}


#if WITH_EDITOR

void UGameDataManager::PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent )
{
	FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if ( PropertyName == GET_MEMBER_NAME_CHECKED( UGameDataManager, bRecollectSoundBanks ) )
	{
		bRecollectSoundBanks = false;
		RecollectSoundBanks();
	}
	else if ( PropertyName == GET_MEMBER_NAME_CHECKED( UGameDataManager, bRecollectGameDataAssets ) )
	{
		bRecollectGameDataAssets = false;
		RecollectGameDataAssetReferences();
	}
	else if ( PropertyName == GET_MEMBER_NAME_CHECKED( UGameDataManager, bRecollectGeneratedClasses ) )
	{
		bRecollectGeneratedClasses = false;
		RecollectGeneratedClassReferences();
	}
	Super::PostEditChangeProperty( PropertyChangedEvent );
}

static bool CheckAssetPath( const FString& AssetPath, const TArray<FString>& IncludePaths, const TArray<FString>& ExcludePaths )
{
	// Check include paths.
	if ( IncludePaths.Num() > 0 )
	{
		bool bFound = false;
		for ( const FString& IncludePath : IncludePaths )
		{
			if ( AssetPath.Contains(IncludePath) )
			{
				bFound = true;
				break;
			}
		}
		if ( !bFound )
		{
			// Failed because the asset path is not listed in the include paths.
			return false;
		}
	}
	// Check Exclude paths.
	if ( ExcludePaths.Num() > 0 )
	{
		bool bFound = false;
		for ( const FString& ExcludePath : ExcludePaths )
		{
			if ( AssetPath.Contains( ExcludePath ) )
			{
				bFound = true;
				break;
			}
		}
		if ( bFound )
		{
			// Failed because the asset path contains one of the exclude paths.
			return false;
		}
	}	

	// Check OK
	return true;
}


void UGameDataManager::RecollectSoundBanks()
{
	// Make sure this function is called from the class default object.
	if ( !HasAnyFlags(RF_ClassDefaultObject) )
	{
		UGameDataManager* DataManager = GetClass()->GetDefaultObject<UGameDataManager>();
		DataManager->RecollectSoundBanks();
		return;
	}

	GetAssetPtrs( SoundBankAssetPtrs );

	Trace( "Recollect sound banks, found %d assets.", SoundBankAssetPtrs.Num() );
}

void UGameDataManager::RecollectGameDataAssetReferences()
{
	// Make sure this function is called from the class default object.
	if ( !HasAnyFlags(RF_ClassDefaultObject) )
	{
		UGameDataManager* DataManager = GetClass()->GetDefaultObject<UGameDataManager>();
		DataManager->RecollectGameDataAssetReferences();
		return;
	}

	// Get all game data asset references
	GetAssetRefs<UGameData>( GameDataAssetReferences, true );

	// Filter asset references if necessary
	bool bHasIncludePaths = GameDataAssetIncludePaths.Num() > 0;
	bool bHasExcludePaths = GameDataAssetExcludePaths.Num() > 0;
	if ( bHasIncludePaths || bHasExcludePaths )
	{
		// This is where the filtered asset references will be stored.
		TArray<FStringAssetReference> FilteredGameDataAssetReferences;
		// Traverse all of the asset references and filtered out based on its asset path.
		for ( int32 i = 0; i < GameDataAssetReferences.Num(); i++ )
		{
			const FStringAssetReference& AssetReference = GameDataAssetReferences[i];
			if ( CheckAssetPath( AssetReference.ToString(), GameDataAssetIncludePaths, GameDataAssetExcludePaths ) )
			{
				// Checking result is OK, so just go ahead and add it to the filtered list.
				FilteredGameDataAssetReferences.Add( AssetReference );
			}
		}

		// Replace the previous list with the filtered one.
		GameDataAssetReferences = FilteredGameDataAssetReferences;
	}
	
	// Mark the package of this object as dirty
	MarkPackageDirty();
}

void UGameDataManager::RecollectGeneratedClassReferences()
{
	// Make sure this function is called from the class default object.
	if ( !HasAnyFlags(RF_ClassDefaultObject) )
	{
		UGameDataManager* DataManager = GetClass()->GetDefaultObject<UGameDataManager>();
		DataManager->RecollectGeneratedClassReferences();
		return;
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<UClass*> AdditionalParentClasses;
	for ( const FStringClassReference& ClassRef : AdditionalParentClassReferences )
	{
		AdditionalParentClasses.Add( ClassRef.ResolveClass() );
	}
	UClass* GameObjectClass = UGameObject::StaticClass();

	GeneratedClassReferences.Empty();

	if ( GeneratedClassIncludePaths.Num() > 0 )
	{
		FARFilter Filter;
		Filter.ClassNames.Add( "Blueprint" );
		for ( const FString& IncludePath : GeneratedClassIncludePaths )
		{
			Filter.PackagePaths.Add( FName( *IncludePath ) );
		}
		Filter.bRecursivePaths = true;

		TArray<FAssetData> AssetDataList;
		AssetRegistry.GetAssets( Filter, AssetDataList );

		for ( const FAssetData& AssetData : AssetDataList )
		{
			UBlueprint* Blueprint = Cast<UBlueprint>( AssetData.GetAsset() );
			if ( Blueprint->ParentClass->IsChildOf( GameObjectClass ) || AdditionalParentClasses.Contains( Blueprint->ParentClass ) )
			{
				FStringClassReference ClassRef = *Blueprint->GeneratedClass;
				GeneratedClassReferences.Add( ClassRef );
			}
		}
	}

	MarkPackageDirty();
}

void UGameDataManager::OnInMemoryAssetCreated( UObject* Asset )
{
	// Make sure this function is called from the class default object.
	if ( !HasAnyFlags(RF_ClassDefaultObject) )
	{
		UGameDataManager* DataManager = GetClass()->GetDefaultObject<UGameDataManager>();
		DataManager->OnInMemoryAssetCreated( Asset );
		return;
	}

	if ( bUseSoundBankCollector )
	{
		if ( USoundBank* SoundBank = Cast<USoundBank>( Asset ) )
		{
			TAssetPtr<USoundBank> AssetPtr = SoundBank;
			SoundBankAssetPtrs.Add( AssetPtr );
		}
	}

	if ( bUseGameDataAssetCollector )
	{
		if ( UGameData* GameData = Cast<UGameData>( Asset ) )
		{
			FString AssetPath;
			AssetPath = Asset->GetPathName( nullptr );
			if ( CheckAssetPath( AssetPath, GameDataAssetIncludePaths, GameDataAssetExcludePaths ) )
			{
				FStringAssetReference AssetRef = GameData;
				GameDataAssetReferences.Add( AssetRef );
				MarkPackageDirty();
				return;
			}
		}
	}
	
	if ( bUseGeneratedClassCollector )
	{
		if ( UBlueprint* Blueprint = Cast<UBlueprint>( Asset ) )
		{
			FString AssetPath;
			AssetPath = Asset->GetPathName( nullptr );
			if ( CheckAssetPath( AssetPath, GeneratedClassIncludePaths, GeneratedClassExcludePaths ) )
			{
				FStringClassReference ParentClassRef = *Blueprint->ParentClass;
				if ( Blueprint->ParentClass->IsChildOf( UGameObject::StaticClass() ) || AdditionalParentClassReferences.Contains( ParentClassRef ) )
				{
					FStringClassReference GeneratedClassRef = *Blueprint->GeneratedClass;
					GeneratedClassReferences.Add( GeneratedClassRef );
					MarkPackageDirty();
					return;
				}
			}
		}
	}
}

void UGameDataManager::OnInMemoryAssetDeleted( UObject* Asset )
{
	// Make sure this function is called from the class default object.
	if ( !HasAnyFlags(RF_ClassDefaultObject) )
	{
		UGameDataManager* DataManager = GetClass()->GetDefaultObject<UGameDataManager>();
		DataManager->OnInMemoryAssetDeleted( Asset );
		return;
	}

	if ( USoundBank* SoundBank = Cast<USoundBank>( Asset ) )
	{
		TAssetPtr<USoundBank> AssetPtr = SoundBank;
		if ( SoundBankAssetPtrs.Remove( AssetPtr ) )
		{
			MarkPackageDirty();
		}
	}
	else if ( UGameData* GameData = Cast<UGameData>( Asset ) )
	{
		FStringAssetReference AssetRef = GameData;
		if ( GameDataAssetReferences.Remove( AssetRef ) )
		{
			MarkPackageDirty();
		}
	}
	else if ( UBlueprint* Blueprint = Cast<UBlueprint>( Asset ) )
	{
		FStringClassReference GeneratedClassRef = *Blueprint->GeneratedClass;
		if ( GeneratedClassReferences.Remove( GeneratedClassRef ) )
		{
			MarkPackageDirty();
		}
	}
}

#endif // WITH_EDITOR
