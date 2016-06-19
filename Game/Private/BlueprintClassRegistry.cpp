// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GamePrivatePCH.h"
#include "BlueprintClassRegistry.h"

#if WITH_EDITOR
#include "Kismet2/KismetEditorUtilities.h"
#include "AssetRegistryModule.h"
#endif

UBlueprintClassRegistry::UBlueprintClassRegistry( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{

}

UClass* UBlueprintClassRegistry::GetParentClass() const
{
	return ParentClass.IsValid() ? ParentClass.ResolveClass() : nullptr;
}

void UBlueprintClassRegistry::GetBlueprintClasses( TArray<UClass*>& OutBlueprintClasses )
{
	UClass* ParentClass = GetParentClass();
	OutBlueprintClasses.Empty();
	for ( const FStringClassReference& ClassRef : BlueprintClasses )
	{
		if ( ClassRef.IsValid() )
		{
			UClass* Class = ClassRef.ResolveClass();
			if ( !Class )
			{
				Class = ClassRef.TryLoadClass<UObject>();
			}
			if ( Class )
			{
				OutBlueprintClasses.Add( Class );
			}
		}
	}
}


void UBlueprintClassRegistry::PostInitProperties()
{
	Super::PostInitProperties();

#if WITH_EDITOR
	if ( IsAsset() )
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
		AssetRegistry.OnInMemoryAssetCreated().AddUObject( this, &UBlueprintClassRegistry::OnInMemoryAssetCreated );
		AssetRegistry.OnInMemoryAssetDeleted().AddUObject( this, &UBlueprintClassRegistry::OnInMemoryAssetDeleted );
	}
#endif

}

#if WITH_EDITOR

bool UBlueprintClassRegistry::CanEditChange( const UProperty* InProperty ) const
{
	if ( Super::CanEditChange(InProperty) )
	{
		if ( InProperty->GetFName() == GET_MEMBER_NAME_CHECKED( UBlueprintClassRegistry, bPopulateBlueprintClasses ) )
		{
			return ParentClass.IsValid();
		}
		return true;
	}

	return false;
}

void UBlueprintClassRegistry::PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent )
{
	static FName PropName_Populate = GET_MEMBER_NAME_CHECKED( UBlueprintClassRegistry, bPopulateBlueprintClasses );
	static FName PropName_ParentClass = GET_MEMBER_NAME_CHECKED( UBlueprintClassRegistry, ParentClass );
	static FName PropName_RecursiveClasses = GET_MEMBER_NAME_CHECKED( UBlueprintClassRegistry, bRecursiveClasses );
	static FName PropName_SearchPaths = GET_MEMBER_NAME_CHECKED( UBlueprintClassRegistry, SearchPaths );

	UProperty* Property = PropertyChangedEvent.Property;
	FName PropertyName = Property->GetFName();

	if ( PropertyName == PropName_Populate )
	{
		if ( bPopulateBlueprintClasses )
		{
			bPopulateBlueprintClasses = false;
			PopulateBlueprintClasses();
		}
	}
	
	Super::PostEditChangeProperty( PropertyChangedEvent );
}


void UBlueprintClassRegistry::PopulateBlueprintClasses()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> AssetDataList;

	FARFilter Filter;
	Filter.ClassNames.Add( UBlueprint::StaticClass()->GetFName() );
	if ( SearchPaths.Num() > 0 )
	{
		Filter.PackagePaths = SearchPaths;
		Filter.bRecursivePaths = true;
	}	
	Filter.bRecursiveClasses = bRecursiveClasses;
	if ( AssetRegistry.GetAssets( Filter, AssetDataList ) )
	{
		UClass* ParentClass = GetParentClass();

		BlueprintClasses.Empty();
		for ( const FAssetData& AssetData : AssetDataList )
		{
			UBlueprint* Blueprint = Cast<UBlueprint>( AssetData.GetAsset() );
			if ( Blueprint && Blueprint->ParentClass == ParentClass )
			{
				FStringClassReference ClassRef;
				ClassRef = *Blueprint->GeneratedClass;
				BlueprintClasses.Add( ClassRef );
				Trace( "Populate-AddClass : %s", *ClassRef.ToString() );
			}
		}
	}
}

void UBlueprintClassRegistry::OnInMemoryAssetCreated( UObject* Object )
{
	UClass* ParentClass = GetParentClass();
	if ( !ParentClass )
	{
		return;
	}

	if ( UBlueprint* Blueprint = Cast<UBlueprint>(Object) )
	{
		UClass* Class = Blueprint->ParentClass;
		if ( Class->IsChildOf(ParentClass) )
		{
			FStringClassReference ClassRef;
			ClassRef = *Blueprint->GeneratedClass;
			BlueprintClasses.Add( ClassRef );
			Trace( "AddClass: %s", *ClassRef.ToString() );
		}
	}
}

void UBlueprintClassRegistry::OnInMemoryAssetDeleted( UObject* Object )
{	
	UClass* ParentClass = GetParentClass();
	if ( !ParentClass )
	{
		return;
	}

	if ( UBlueprint* Blueprint = Cast<UBlueprint>(Object) )
	{
		UClass* Class = Blueprint->ParentClass;
		if ( Class->IsChildOf(ParentClass) )
		{
			FStringClassReference ClassRef;
			ClassRef = *Blueprint->GeneratedClass;
			if ( BlueprintClasses.Remove( ClassRef ) > 0 )
			{
				Trace( "RemoveClass: %s", *ClassRef.ToString() );
			}
		}
	}
}

#endif
