// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "Factories/Factory.h"
#include "GameObjectFactory.generated.h"

class UGameObject;

UCLASS(hidecategories=Object, MinimalAPI)
class UGameObjectFactory : public UFactory
{
	GENERATED_BODY()
public:

	UGameObjectFactory(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category="GObject")
	TSubclassOf<UGameObject> GameObjectClass;


	// UFactory interface
	virtual uint32 GetMenuCategories() const;
	virtual FText GetDisplayName() const override;
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;	
	// End of UFactory interface
};