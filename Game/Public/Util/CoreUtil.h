/** @file CoreUtil.h
* Contains a mostly macros and template functions that only relies on UE's Core and CoreUObject modules.
* @note: (Optional) Please define PRINT_LOG_CATEGORY and TRACE_LOG_CATEGORY or LogTemp will be used instead.
*		 (Optional) Please define PRINT_MESSAGE_DURATION or 5.f will be used
*		 (Optional) Please define LOCTEXT_NAMESPACE or "Game" will be used
* @author Fathurahman <ipat.bogor@gmail.com>
*/

#pragma once

//////////////////////////////////////////////////////////////////////////
// UE_LOG and ScreenMessage utilities
//////////////////////////////////////////////////////////////////////////

#ifndef PRINT_LOG_CATEGORY
#define PRINT_LOG_CATEGORY LogTemp
#endif

#ifndef TRACE_LOG_CATEGORY
#define TRACE_LOG_CATEGORY LogTemp
#endif

#ifndef PRINT_MESSAGE_DURATION
#define PRINT_MESSAGE_DURATION 5.f
#endif

// Current file and line number
#define STR_CUR_FILE_LINE (FString(__FILE__).Right(FString(__FILE__).Len() - FString(__FILE__).Find(TEXT("\\"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) - 1) + " (" + FString::Printf( TEXT("%4d"), __LINE__) + ")")

// Print macros
#define PrintLog(Format, ...)			UE_LOG( PRINT_LOG_CATEGORY, Log,		TEXT("%s: "Format), *STR_CUR_FILE_LINE, ##__VA_ARGS__ )
#define PrintLogWarning(Format, ...)	UE_LOG( PRINT_LOG_CATEGORY, Warning,	TEXT("%s: "Format), *STR_CUR_FILE_LINE, ##__VA_ARGS__ )
#define PrintLogError(Format, ...)		UE_LOG( PRINT_LOG_CATEGORY, Error,		TEXT("%s: "Format), *STR_CUR_FILE_LINE, ##__VA_ARGS__ )
#define PrintLogFatal(Format, ...)		UE_LOG( PRINT_LOG_CATEGORY, Fatal,		TEXT("%s: "Format), *STR_CUR_FILE_LINE, ##__VA_ARGS__ )

// Trace macros
#ifdef UE_BUILD_DEBUG
	#define Trace(Format, ...)			UE_LOG( TRACE_LOG_CATEGORY, Warning,	TEXT("%s: "Format), *STR_CUR_FILE_LINE, ##__VA_ARGS__ )
#else
	#define Trace(Format, ...)
#endif

// Print message to screen macro
#define PrintMessage(Color, Format, ...)	GEngine->AddOnScreenDebugMessage( -1, PRINT_MESSAGE_DURATION, Color, *FString::Printf( TEXT("%s: "Format), *STR_CUR_FILE_LINE, ##__VA_ARGS__ ) )

#ifndef LOCTEXT_NAMESPACE
#define LOCTEXT_NAMESPACE "Game"
#endif

#define LTEXT(TXT) NSLOCTEXT( LOCTEXT_NAMESPACE, TXT, TXT )

//////////////////////////////////////////////////////////////////////////
// Array utilities
//////////////////////////////////////////////////////////////////////////

/** 
* Reinterpret an array of object from one object class to another.
* @param[in]	SrcArray	Source array to reinterpret.
* @param[out]	DstArray	Destination array where the casted objects will be stored.
*/
template<class SrcType=UObject, class DstType=UObject>
FORCEINLINE void ReinterpretObjectArray(const TArray<SrcType*>& SrcArray, TArray<DstType*>& DstArray) 
{	
	DstArray.Empty();

	if ( SrcArray.Num() = 0 )
	{
		return;
	}

	DstArray.Empty();
	DstArray.Reserve( SrcArray.Num() );

	for (SrcType* Object : SrcArray)
	{
		if ( DstType* CastedObject = Cast<DstType>(Object) )
		{
			DstArray.Add( Cast<DstType>( CastedObject ) );
		}		
	}
}

/**
* Clean an object array, it removes all invalid objects from the array.
* @param	Array	Object array to be cleaned.
* @return true if the array was cleaned or false if there's nothing need to be cleaned in the array.
*/
template<class T=UObject>
FORCEINLINE bool CleanObjectArray( TArray<T*>& Array )
{
	if ( Array.Num() == 0 )
	{
		return false;
	}
	TArray<T*> CleanedArray;
	CleanedArray.Reserve( Array.Num() );

	TWeakObjectPtr ObjectPtr;
	for ( T* Object : Array )
	{
		ObjectPtr = Object;
		if ( ObjectPtr.IsValid() )
		{
			CleanedArray.Add( Object );
		}
	}

	if ( CleanedArray.Num() != Array.Num() )
	{
		Array = CleanedArray;
		return true;
	}

	return false;
}

/** 
* Helper function to add an entry to an array
*/
template<typename T>
FORCEINLINE static TArray<T>& operator<< ( TArray<T>& Array, const T& Entry )
{
	Array.Add( Entry );
	return Array;
}

/** 
* Helper function to add an entry to an array of objects
*/
template<class T=UObject>
FORCEINLINE static TArray<T*>& operator<< ( TArray<T*>& Array, T* Entry )
{
	Array.Add( Entry );
	return Array;
}

//////////////////////////////////////////////////////////////////////////
// Map (Data structure) Utilities
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Miscellaneous
//////////////////////////////////////////////////////////////////////////

/** 
* Utility function to get or load an asset from an asset reference.
* @Note: This function will load the asset synchronously if the asset is not loaded,
*		 which means it will take a while if the asset size is very big.	
* @param[in]	AssetRef	Asset reference where the asset will be taken from
* @return nullptr if the asset pointed to by AssetRef is not valid.
*/
template<class T=UObject>
FORCEINLINE T* GetOrLoadAssetFromRef( const FStringAssetReference& AssetRef )
{
	UObject* Object = AssetRef.ResolveObject();
	if ( !Object )
	{
		Object = AssetRef.TryLoad();
	}
	return Object ? Cast<T>( Object ) : nullptr;
}

/**
* Utility function to get or load an asset from an asset pointer.
* @Note: This function will load the asset synchronously if the asset is not loaded,
*		 which means it will take a while if the asset size is very big.	
* @param[in]	AssetPtr	Asset pointer where the asset will be taken from
* @return nullptr if the asset pointed to by AssetPtr is not valid.
*/
template<class T>
FORCEINLINE T* GetOrLoadAssetFromPtr(const TAssetPtr<T>& AssetPtr)
{
	TAssetPtr<T> AssetPtrCopy = AssetPtr;
	if ( AssetPtrCopy.IsNull() )
	{
		// This asset pointer doesn't point to an asset.
		return nullptr;
	}
	if ( AssetPtrCopy.IsValid() == false )
	{
		// This asset pointer points to an asset but it has not been loaded.
		AssetPtrCopy.LoadSynchronous();
	}
	return AssetPtrCopy.Get();
}