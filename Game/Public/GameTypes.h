// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "GameTypes.generated.h"

//////////////////////////////////////////////////////////////////////////
// Enums
//////////////////////////////////////////////////////////////////////////

UENUM(BlueprintType)
enum class EValidity: uint8
{
    Valid,
	NotValid
};

UENUM(BlueprintType)
enum class EYesNo : uint8
{
	Yes,
	No
};

UENUM(BlueprintType)
enum class ETagFilterType : uint8
{
	HasAnyTags,
	HasAllTags
};

UENUM(BlueprintType)
enum class ESuccess : uint8
{
	Success,
	Fail
};

UENUM(BlueprintType)
enum class EFindResult : uint8
{
    Found,
	NotFound
};

UENUM(BlueprintType)
enum class EImageFileFormat : uint8
{
	/** Invalid or unrecognized format */
	Invalid = 0,

	/** Portable Network Graphics */
	PNG,

	/** Joint Photographic Experts Group */
	JPEG,

	/** Single channel jpeg */
	GrayscaleJPEG,	

	/** Windows Bitmap */
	BMP,

	/** Windows Icon resource */
	ICO,

	/** OpenEXR (HDR) image file format */
	EXR,

	/** Mac icon */
	ICNS    
};

//////////////////////////////////////////////////////////////////////////
// Structs
//////////////////////////////////////////////////////////////////////////

USTRUCT()
struct GAME_API FNamedData
{
    GENERATED_USTRUCT_BODY()
    
	UPROPERTY()
	FString Name;
       
};

USTRUCT()
struct GAME_API FNamedBoolean : public FNamedData
{
    GENERATED_USTRUCT_BODY()
    
	UPROPERTY()
	bool Value = false;

	FNamedBoolean() {}

	FNamedBoolean( const FString& InName, bool InValue )
	{
		Name = InName;
		Value = InValue;
	}
    
};

USTRUCT()
struct GAME_API FNamedInteger : public FNamedData
{
    GENERATED_USTRUCT_BODY()
    
	UPROPERTY()
	int32 Value = 0; 

	FNamedInteger() {}

	FNamedInteger( const FString& InName, int32 InValue )
	{
		Name = InName;
		Value = InValue;
	}
};

USTRUCT()
struct GAME_API FNamedFloat : public FNamedData
{
    GENERATED_USTRUCT_BODY()
    
	UPROPERTY()
    float Value = 0.f;

	FNamedFloat() {}

	FNamedFloat( const FString& InName, float InValue )
	{
		Name = InName;
		Value = InValue;
	}
};

USTRUCT()
struct GAME_API FNamedString : public FNamedData
{
    GENERATED_USTRUCT_BODY()
    
	UPROPERTY(SaveGame)
	FString Value;
    
	FNamedString() {}

	FNamedString( const FString& InName, const FString& InValue )
	{
		Name = InName;
		Value = InValue;
	}
};

/** Base struct for random weighted entry. */
USTRUCT(BlueprintType)
struct GAME_API FRandomWeightedStruct
{
    GENERATED_USTRUCT_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game", meta=(UIMin="1"))
    int32 RandomWeight = 10;
    
};

struct GAME_API FSaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
	FSaveGameArchive(FArchive& InInnerArchive)
		: FObjectAndNameAsStringProxyArchive(InInnerArchive, true)
	{
		ArIsSaveGame = true;
	}
};

//////////////////////////////////////////////////////////////////////////
// Save Game Records
//////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct GAME_API FGameObjectRecord
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	UClass* Class;

	UPROPERTY()
	FName Name;

	UPROPERTY()
	FString ID;

	UPROPERTY()
	TArray<uint8> ByteData;
};

USTRUCT(BlueprintType)
struct GAME_API FGameObjectLinkRecord
{
    GENERATED_USTRUCT_BODY()
    
	UPROPERTY()
	FName ParentName;

	UPROPERTY()
	TArray<FName> ChildNames;
};

USTRUCT(BlueprintType)
struct GAME_API FGameObjectTreeRecord
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FGameObjectRecord> ObjectRecords;

	UPROPERTY()
	TArray<FGameObjectLinkRecord> LinkRecords;

	UPROPERTY()
	TArray<uint8> ByteData;
};

USTRUCT(BlueprintType)
struct GAME_API FGameRecord
{
    GENERATED_USTRUCT_BODY()
    
	UPROPERTY()
	FGameObjectTreeRecord ObjectTreeRecord;

	UPROPERTY()
	TArray<uint8> ByteData;

	UPROPERTY()
	TArray<FNamedBoolean> GameFlags;

	UPROPERTY()
	TArray<FNamedInteger> GameInts;

	UPROPERTY()
	TArray<FNamedFloat> GameFloats;

	UPROPERTY()
	TArray<FNamedString> GameStrings;
};