// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once
#include "Factories/Factory.h"
#include "GUIWidgetFactory.generated.h"

class UGUIWidget;

UCLASS(HideCategories=Object, MinimalAPI)
class UGUIWidgetFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(EditAnywhere, Category="GUIWidget")
	TSubclassOf<UGUIWidget> GUIWidgetClass;

	virtual FText GetDisplayName() const override;
	virtual bool ConfigureProperties() override;
	virtual bool ShouldShowInNewMenu() const override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;	

};