// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GameEdPrivatePCH.h"
#include "Engine/Blueprint.h"
#include "GameClassViewerFilter.h"
#include "Factories/BlueprintFactory.h"
#include "KismetCompilerModule.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "GameObjectBlueprintFactory.h"
#include "AssetTypeCategories.h"


UGameObjectBlueprintFactory::UGameObjectBlueprintFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UBlueprint::StaticClass();
}

uint32 UGameObjectBlueprintFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Blueprint;
}

FText UGameObjectBlueprintFactory::GetDisplayName() const
{
	return LOCTEXT("GameObjectBlueprint", "Game Object Blueprint");
}

// Show class picker dialog for use to pick which data object class he/she wants to create.
bool UGameObjectBlueprintFactory::ConfigureProperties()
{
	// Get Class viewer module
	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	// Setup class viewer filter
	TSharedPtr<FGClassViewerFilter> Filter = MakeShareable(new FGClassViewerFilter());
	Filter->DisallowedClassFlags = CLASS_CompiledFromBlueprint | CLASS_Deprecated | CLASS_NewerVersionExists;
	Filter->AllowedChildrenOfClasses.Add(UGameObject::StaticClass());
	Filter->bAllowBlueprintClasses = false;

	// Setup class viewer options
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.DisplayMode = EClassViewerDisplayMode::ListView;
	Options.ClassFilter = Filter;

	// Only want blueprint actor base classes.
	Options.bIsBlueprintBaseOnly = true;

	// This will allow unloaded blueprints to be shown.
	Options.bShowUnloadedBlueprints = false;

	// Enable Class Dynamic Loading
	Options.bEnableClassDynamicLoading = false;

	// Show the class picker and save/return the result
	ParentClass = nullptr;
	const FText TitleText = LOCTEXT("PickGameObjectParentClass", "Pick Game Object Parent class");
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass( TitleText, Options, ChosenClass, UGameObject::StaticClass() );
	if ( bPressedOk )
	{
		ParentClass = ChosenClass;
	}
	return bPressedOk;
}

// Create the data object, make sure it's the correct class and enforce public + transactional flag because it must be an asset.
UObject* UGameObjectBlueprintFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	// Make sure we are trying to factory a blueprint, then create and init one
	check(Class->IsChildOf(UBlueprint::StaticClass()));

	if ((ParentClass == nullptr) || !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass))
	{
		FFormatNamedArguments Args;
		Args.Add( TEXT("ClassName"), (ParentClass != nullptr) ? FText::FromString( ParentClass->GetName() ) : LOCTEXT("Null", "(null)") );
		FMessageDialog::Open( EAppMsgType::Ok, FText::Format( LOCTEXT("CannotCreateBlueprintFromClass", "Cannot create a blueprint based on the class '{0}'."), Args ) );
		return nullptr;
	}
	else
	{
		UClass* BlueprintClass = nullptr;
		UClass* BlueprintGeneratedClass = nullptr;

		IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
		KismetCompilerModule.GetBlueprintTypesForClass(ParentClass, BlueprintClass, BlueprintGeneratedClass);

		return FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, Name, BPTYPE_Normal, BlueprintClass, BlueprintGeneratedClass, CallingContext);
	}
}

UObject* UGameObjectBlueprintFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
}
