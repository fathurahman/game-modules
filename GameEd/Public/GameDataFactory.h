// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "Factories/Factory.h"
#include "GameDataFactory.generated.h"

class UGameData;

UCLASS(hidecategories=Object, MinimalAPI)
class UGameDataFactory : public UFactory
{
	GENERATED_BODY()
public:

	UGameDataFactory(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category="GObject")
	TSubclassOf<UGameData> DataObjectClass;


	// UFactory interface
	virtual uint32 GetMenuCategories() const;
	virtual FText GetDisplayName() const override;
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;	
	// End of UFactory interface
};