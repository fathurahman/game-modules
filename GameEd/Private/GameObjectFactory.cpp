// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GameEdPrivatePCH.h"
#include "GameClassViewerFilter.h"
#include "GameObjectFactory.h"

UGameObjectFactory::UGameObjectFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UGameObject::StaticClass();
}

uint32 UGameObjectFactory::GetMenuCategories() const
{
	return GameAssetCategory;
}

FText UGameObjectFactory::GetDisplayName() const
{
	return LOCTEXT("GameObjectAsset", "Game Object");
}

// Show class picker dialogue for use to pick which data object class he/she wants to create.
bool UGameObjectFactory::ConfigureProperties()
{
	// Get Class viewer module
	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	// Setup class viewer filter
	TSharedPtr<FGClassViewerFilter> Filter = MakeShareable(new FGClassViewerFilter());
	Filter->DisallowedClassFlags = CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists;
	Filter->AllowedChildrenOfClasses.Add(UGameObject::StaticClass());
	Filter->bAllowBlueprintClasses = true;

	// Setup class viewer options
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.ClassFilter = Filter;

	// Show the class picker and save/return the result
	GameObjectClass = nullptr;
	const FText TitleText = LOCTEXT("PickGameObjectClass", "Pick Game Object class");
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass( TitleText, Options, ChosenClass, UGameObject::StaticClass() );
	if ( bPressedOk )
	{
		GameObjectClass = ChosenClass;
	}
	return bPressedOk;
}

// Create the data object, make sure it's the correct class and enforce public + transactional flag because it must be an asset.
UObject* UGameObjectFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (GameObjectClass != nullptr)
	{
		Class = GameObjectClass;
	}

	check(Class->IsChildOf(UGameObject::StaticClass()));

	UE_LOG( LogGameObjectEd, Warning, TEXT("Flags = %X"), (int32)Flags );

	return NewObject<UGameObject>( InParent, Class, Name, Flags | RF_Transactional );
}
