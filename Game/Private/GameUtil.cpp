// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GamePrivatePCH.h"
#include "Object/GameObjectTree.h"
#include "Util/GameUtil.h"
#include "Framework/Game.h"
#include "Framework/GameManager.h"
#include "Interfaces/IImageWrapperModule.h"
#include "Util/BlueprintClassRegistry.h"

UBlueprintClassRegistry* GetBlueprintClassRegistry( UClass* ParentClass )
{
	TArray<UBlueprintClassRegistry*> BlueprintClassRegistries;
	GetAssets( BlueprintClassRegistries );
	for ( UBlueprintClassRegistry* Reg : BlueprintClassRegistries )
	{
		if ( Reg->GetParentClass() == ParentClass )
		{
			return Reg;
		}
	}
	return nullptr;
}

void GetClasses( UClass* ClassToSearch, TArray<UClass*>& OutResult, bool bAllowAbstract, bool bBlueprintClassOnly )
{	
	OutResult.Empty();

	if ( !ClassToSearch )
	{
		return;
	}

	UBlueprintClassRegistry* BlueprintClassRegistry = GetBlueprintClassRegistry( ClassToSearch );
	if ( BlueprintClassRegistry )
	{
		BlueprintClassRegistry->GetBlueprintClasses( OutResult );
	}

	if ( bBlueprintClassOnly )
	{
		return;
	}

	for ( TObjectIterator<UClass> It; It; ++It )
	{
		UClass* Class = *It;
		if ( Class->HasAnyClassFlags(CLASS_Native) )
		{
			if ( bAllowAbstract == false )
			{
				if ( Class->HasAnyClassFlags(CLASS_Abstract) )
				{
					continue;
				}
			}

			if ( Class->IsChildOf( ClassToSearch ) )
			{
				OutResult.Add( Class );
			}
		}
	}
}

UGameObject* StaticCreateGameObject( TSubclassOf<UGameObject> Class )
{
	UObject* Outer = (UObject*) GetTransientPackage();
	UGameObject* GameObject = NewObject<UGameObject>( Outer, Class );
	return GameObject;
}

bool PlaySoundBankEntry(const FString& SoundBankID, const FString& SoundID, UWorld* World )
{
	UGameManager* GameInstance = UGameManager::Get();
	if ( !GameInstance )
	{
		return false;
	}
	USoundBank* SoundBank = GameInstance->GetSoundBank( SoundBankID );
	if ( !SoundBank )
	{
		return false;
	}
	
	return SoundBank->Play( SoundID, World );
}

bool PlaySoundBankEntry( const USoundBank* SoundBank, const FString& SoundID, UWorld* World )
{
	if ( !SoundBank )
	{
		return false;
	}
	return SoundBank->Play( SoundID, World );
}

bool PlaySoundBankEntryAtLocation( const FString& SoundBankID, const FString& SoundID, const FVector& Location, const FRotator& Rotation, UWorld* World )
{
	UGameManager* GameInstance = UGameManager::Get();
	if ( !GameInstance )
	{
		return false;
	}
	USoundBank* SoundBank = GameInstance->GetSoundBank( SoundBankID );
	if ( !SoundBank )
	{
		return false;
	}
	return SoundBank->PlayAtLocation( SoundID, Location, Rotation, World );
}

UAudioComponent* PlaySoundBankEntryAttached(const FString& SoundBankID, const FString& SoundID, class USceneComponent* AttachToComponent, const FName& AttachPointName, const FVector& Location, const FRotator& Rotation, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroyed)
{
	UGameManager* GameInstance = UGameManager::Get();
	if ( !GameInstance )
	{
		return nullptr;
	}
	USoundBank* SoundBank = GameInstance->GetSoundBank( SoundBankID );
	if ( !SoundBank )
	{
		return nullptr;
	}
	return SoundBank->PlayAttached( SoundID, AttachToComponent, AttachPointName, Location, Rotation, LocationType, bStopWhenAttachedToDestroyed );
}

UTexture2D* LoadTexture2DFromFile( const FString& FilePath, EImageFileFormat ImageFileFormat, int32& OutWidth, int32& OutHeight )
{
	UTexture2D* Texture = nullptr;
	
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

	EImageFormat::Type ImageFormat = EImageFormat::Type( int32((uint8) ImageFileFormat) - 1 );
	
	IImageWrapperPtr ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
 
	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *FilePath)) 
	{
		return nullptr;
	}	
	  
	//Create T2D!
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
	{ 
		const TArray<uint8>* UncompressedBGRA = NULL;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
		{
			Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);			
			if (!Texture) 
			{
				return nullptr;
			}
			
			OutWidth = ImageWrapper->GetWidth();
			OutHeight = ImageWrapper->GetHeight();
			 
			void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, UncompressedBGRA->GetData(), UncompressedBGRA->Num());
			Texture->PlatformData->Mips[0].BulkData.Unlock();

			Texture->UpdateResource();
		}
	}
	 
	return Texture;
	
}

#define TestAndAddFlag(Result, Flags, Flag) {\
	if ( Flags & Flag ) \
	{ \
		Result += TEXT(#Flag); \
		Result += TEXT(" "); \
	} \
}

FString ObjectFlagsToString( EObjectFlags ObjectFlags )
{
	FString Result;

	// This first group of flags mostly has to do with what kind of object it is. Other than transient, these are the persistent object flags.
	// The garbage collector also tends to look at these.
	TestAndAddFlag(Result, ObjectFlags, RF_Public);	///< Object is visible outside its package.
	TestAndAddFlag(Result, ObjectFlags, RF_Standalone);	///< Keep object around for editing even if unreferenced.
	TestAndAddFlag(Result, ObjectFlags, RF_MarkAsNative);	///< Object (UField) will be marked as native on construction (DO NOT USE THIS FLAG in HasAnyFlags() etc)
	TestAndAddFlag(Result, ObjectFlags, RF_Transactional);	///< Object is transactional.
	TestAndAddFlag(Result, ObjectFlags, RF_ClassDefaultObject);	///< This object is its class's default object
	TestAndAddFlag(Result, ObjectFlags, RF_ArchetypeObject);	///< This object is a template for another object - treat like a class default object
	TestAndAddFlag(Result, ObjectFlags, RF_Transient);	///< Don't save object.

	// This group of flags is primarily concerned with garbage collection.
	TestAndAddFlag(Result, ObjectFlags, RF_MarkAsRootSet);	///< Object will be marked as root set on construction and not be garbage collected, even if unreferenced (DO NOT USE THIS FLAG in HasAnyFlags() etc)
	TestAndAddFlag(Result, ObjectFlags, RF_TagGarbageTemp);	///< This is a temp user flag for various utilities that need to use the garbage collector. The garbage collector itself does not interpret it.

	// The group of flags tracks the stages of the lifetime of a uobject
	TestAndAddFlag(Result, ObjectFlags, RF_NeedLoad);	///< During load, indicates object needs loading.
	TestAndAddFlag(Result, ObjectFlags, RF_NeedPostLoad);	///< Object needs to be postloaded.
	TestAndAddFlag(Result, ObjectFlags, RF_NeedPostLoadSubobjects);	///< During load, indicates that the object still needs to instance subobjects and fixup serialized component references
	TestAndAddFlag(Result, ObjectFlags, RF_BeginDestroyed);	///< BeginDestroy has been called on the object.
	TestAndAddFlag(Result, ObjectFlags, RF_FinishDestroyed);	///< FinishDestroy has been called on the object.

	// Misc. Flags
	TestAndAddFlag(Result, ObjectFlags, RF_BeingRegenerated);	///< Flagged on UObjects that are used to create UClasses (e.g. Blueprints) while they are regenerating their UClass on load (See FLinkerLoad::CreateExport())
	TestAndAddFlag(Result, ObjectFlags, RF_DefaultSubObject);	///< Flagged on subobjects that are defaults
	TestAndAddFlag(Result, ObjectFlags, RF_WasLoaded);	///< Flagged on UObjects that were loaded
	TestAndAddFlag(Result, ObjectFlags, RF_TextExportTransient);	///< Do not export object to text form (e.g. copy/paste). Generally used for sub-objects that can be regenerated from data in their parent object.
	TestAndAddFlag(Result, ObjectFlags, RF_LoadCompleted);	///< Object has been completely serialized by linkerload at least once. DO NOT USE THIS FLAG, It should be replaced with RF_WasLoaded.
	TestAndAddFlag(Result, ObjectFlags, RF_InheritableComponentTemplate); ///< Archetype of the object can be in its super class
	TestAndAddFlag(Result, ObjectFlags, RF_StrongRefOnFrame);	///< References to this object from persistent function frame are handled as strong ones.
	TestAndAddFlag(Result, ObjectFlags, RF_Dynamic); //< Field Only. Dynamic field - doesn't get constructed during static initialization, can be constructed multiple times

	return Result;
}

FString ClassFlagsToString( uint32 ClassFlags )
{
	FString Result;

	/** Class is abstract and can't be instantiated directly. */
	TestAndAddFlag(Result, ClassFlags, CLASS_Abstract);
	/** Save object configuration only to Default INIs, never to local INIs. Must be combined with CLASS_Config */
	TestAndAddFlag(Result, ClassFlags, CLASS_DefaultConfig);
	/** Load object configuration at construction time. */
	TestAndAddFlag(Result, ClassFlags, CLASS_Config);
	/** This object type can't be saved; null it out at save time. */
	TestAndAddFlag(Result, ClassFlags, CLASS_Transient);
	/** Successfully parsed. */
	TestAndAddFlag(Result, ClassFlags, CLASS_Parsed);
	/** */
	//CLASS_                  = 0x00000020,
	/** All the properties on the class are shown in the advanced section (which is hidden by default) unless SimpleDisplay is specified on the property */
	TestAndAddFlag(Result, ClassFlags, CLASS_AdvancedDisplay);
	/** Class is a native class - native interfaces will have CLASS_Native set, but not RF_MarkAsNative */
	TestAndAddFlag(Result, ClassFlags, CLASS_Native);
	/** Don't export to C++ header. */
	TestAndAddFlag(Result, ClassFlags, CLASS_NoExport);
	/** Do not allow users to create in the editor. */
	TestAndAddFlag(Result, ClassFlags, CLASS_NotPlaceable);
	/** Handle object configuration on a per-object basis, rather than per-class. */
	TestAndAddFlag(Result, ClassFlags, CLASS_PerObjectConfig);
	
	/** pointers to this class default to weak. */
	TestAndAddFlag(Result, ClassFlags, CLASS_PointersDefaultToWeak);
	
	/** Class can be constructed from editinline New button. */
	TestAndAddFlag(Result, ClassFlags, CLASS_EditInlineNew);
	/** Display properties in the editor without using categories. */
	TestAndAddFlag(Result, ClassFlags, CLASS_CollapseCategories);
	/** Class is an interface **/
	TestAndAddFlag(Result, ClassFlags, CLASS_Interface);
	/**  Do not export a constructor for this class, assuming it is in the cpptext **/
	TestAndAddFlag(Result, ClassFlags, CLASS_CustomConstructor);
	/** all properties and functions in this class are const and should be exported as const */
	TestAndAddFlag(Result, ClassFlags, CLASS_Const);

	/** pointers to this class default to weak. */
	TestAndAddFlag(Result, ClassFlags, CLASS_PointersDefaultToAutoWeak);
	
	/** Indicates that the class was created from blueprint source material */
	TestAndAddFlag(Result, ClassFlags, CLASS_CompiledFromBlueprint);

	/** Indicates that only the bare minimum bits of this class should be DLL exported/imported */
	TestAndAddFlag(Result, ClassFlags, CLASS_MinimalAPI);
	
	/** Indicates this class must be DLL exported/imported (along with all of it's members) */
	TestAndAddFlag(Result, ClassFlags, CLASS_RequiredAPI);

	/** Indicates that references to this class default to instanced. Used to be subclasses of UComponent, but now can be any UObject */
	TestAndAddFlag(Result, ClassFlags, CLASS_DefaultToInstanced);

	/** Indicates that the parent token stream has been merged with ours. */
	TestAndAddFlag(Result, ClassFlags, CLASS_TokenStreamAssembled);
	/** Class has component properties. */
	TestAndAddFlag(Result, ClassFlags, CLASS_HasInstancedReference);
	/** Don't show this class in the editor class browser or edit inline new menus. */
	TestAndAddFlag(Result, ClassFlags, CLASS_Hidden);
	/** Don't save objects of this class when serializing */
	TestAndAddFlag(Result, ClassFlags, CLASS_Deprecated);
	/** Class not shown in editor drop down for class selection */
	TestAndAddFlag(Result, ClassFlags, CLASS_HideDropDown);
	/** Class settings are saved to <AppData>/..../Blah.ini (as opposed to CLASS_DefaultConfig) */
	TestAndAddFlag(Result, ClassFlags, CLASS_GlobalUserConfig);
	/** Class was declared directly in C++ and has no boilerplate generated by UnrealHeaderTool */
	TestAndAddFlag(Result, ClassFlags, CLASS_Intrinsic);
	/** Class has already been constructed (maybe in a previous DLL version before hot-reload). */
	TestAndAddFlag(Result, ClassFlags, CLASS_Constructed);
	/** Indicates that object configuration will not check against ini base/defaults when serialized */
	TestAndAddFlag(Result, ClassFlags, CLASS_ConfigDoNotCheckDefaults);
	/** Class has been consigned to oblivion as part of a blueprint recompile, and a newer version currently exists. */
	TestAndAddFlag(Result, ClassFlags, CLASS_NewerVersionExists);

	return Result;
}
