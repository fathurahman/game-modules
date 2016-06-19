// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GameEdPrivatePCH.h"
#include "GameClassViewerFilter.h"
#include "GameDataFactory.h"
#include "AssetTypeCategories.h"

UGameDataFactory::UGameDataFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UGameData::StaticClass();
}

uint32 UGameDataFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Misc;
}

FText UGameDataFactory::GetDisplayName() const
{
	return LOCTEXT("GameDataAsset", "Game Data");
}

// Show class picker dialogue for use to pick which data object class he/she wants to create.
bool UGameDataFactory::ConfigureProperties()
{
	// Get Class viewer module
	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	// Setup class viewer filter
	TSharedPtr<FGClassViewerFilter> Filter = MakeShareable(new FGClassViewerFilter());
	Filter->DisallowedClassFlags = CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists;
	Filter->AllowedChildrenOfClasses.Add(UGameData::StaticClass());
	Filter->bAllowBlueprintClasses = true;

	// Setup class viewer options
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.ClassFilter = Filter;
	Options.DisplayMode = EClassViewerDisplayMode::TreeView;
	Options.bShowObjectRootClass = false;
	Options.bShowUnloadedBlueprints = true;
	Options.bExpandRootNodes = true;

	// Show the class picker and save/return the result
	DataObjectClass = nullptr;
	const FText TitleText = LOCTEXT("PickGameDataClass", "Pick Game Data Class");
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass( TitleText, Options, ChosenClass, UGameData::StaticClass() );
	if ( bPressedOk )
	{
		DataObjectClass = ChosenClass;
	}
	return bPressedOk;
}

// Create the data object, make sure it's the correct class and enforce public + transactional flag because it must be an asset.
UObject* UGameDataFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (DataObjectClass != nullptr)
	{
		Class = DataObjectClass;
	}

	check(Class->IsChildOf(UGameData::StaticClass()));

	InParent->IsAsset();

	return NewObject<UGameData>( InParent, Class, Name, Flags | RF_Transactional );
}
