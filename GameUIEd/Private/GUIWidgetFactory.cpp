// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GameUIEdPrivate.h"

#include "UnrealEd.h"
#include "WidgetBlueprint.h"
#include "ClassViewerModule.h"
#include "ClassViewerFilter.h"
#include "Engine/Blueprint.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/SClassPickerDialog.h"
#include "Logging/MessageLog.h"
#include "GUIWidget.h"

#include "GUIWidgetFactory.h"

class FGUIWidgetClassFilter : public IClassViewerFilter
{
public:
	/** All children of these classes will be included unless filtered out by another setting. */
	TSet< const UClass* > AllowedChildrenOfClasses;

	/** Disallowed class flags. */
	uint32 DisallowedClassFlags;

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs ) override
	{
		UClass* GUIWidgetClass = UGUIWidget::StaticClass();
		return InClass == GUIWidgetClass || InClass->IsChildOf(GUIWidgetClass);
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		UClass* GUIWidgetClass = UGUIWidget::StaticClass();
		return InUnloadedClassData->IsChildOf(GUIWidgetClass);
	}
};


UGUIWidgetFactory::UGUIWidgetFactory(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UWidgetBlueprint::StaticClass();
}

FText UGUIWidgetFactory::GetDisplayName() const
{
	return NSLOCTEXT("GameUIEd", "GUIWidgetBlueprint", "GUI Widget Blueprint");
}

bool UGUIWidgetFactory::ConfigureProperties()
{
	// nullptr the GUIWidgetClass so we can check for selection
	GUIWidgetClass = nullptr;

	// Load the classviewer module to display a class picker
	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	// Fill in options
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.DisplayMode = EClassViewerDisplayMode::DefaultView;
	Options.bShowObjectRootClass = false;

	TSharedPtr<FGUIWidgetClassFilter> Filter = MakeShareable(new FGUIWidgetClassFilter);
	Options.ClassFilter = Filter;


	const FText TitleText = NSLOCTEXT("GameUIEd", "CreateGUIWidget", "Pick GUI Widget Class");
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, UGUIWidget::StaticClass());

	if ( bPressedOk )
	{
		GUIWidgetClass = ChosenClass;
	}

	return bPressedOk;
}

bool UGUIWidgetFactory::ShouldShowInNewMenu() const
{
	return true;
}

UObject* UGUIWidgetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	// Make sure we are trying to factory a Anim Blueprint, then create and init one
	check(Class->IsChildOf(UWidgetBlueprint::StaticClass()));

	if ( ( GUIWidgetClass == NULL ) || !FKismetEditorUtilities::CanCreateBlueprintOfClass(GUIWidgetClass) || !GUIWidgetClass->IsChildOf(UUserWidget::StaticClass()) )
	{
		FFormatNamedArguments Args;
		Args.Add( TEXT("ClassName"), (GUIWidgetClass != NULL) ? FText::FromString( GUIWidgetClass->GetName() ) : NSLOCTEXT("GameUIEd", "Null", "(null)") );
		FMessageDialog::Open( EAppMsgType::Ok, FText::Format( NSLOCTEXT("GameUIEd", "CannotCreateWidgetBlueprint", "Cannot create a GUI Widget Blueprint based on the class '{ClassName}'."), Args ) );
		return NULL;
	}
	else
	{
		UWidgetBlueprint* NewBP = CastChecked<UWidgetBlueprint>(FKismetEditorUtilities::CreateBlueprint(GUIWidgetClass, InParent, Name, BPTYPE_Normal, UWidgetBlueprint::StaticClass(), UWidgetBlueprintGeneratedClass::StaticClass(), CallingContext));
		return NewBP;
	}
}

UObject* UGUIWidgetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
}
