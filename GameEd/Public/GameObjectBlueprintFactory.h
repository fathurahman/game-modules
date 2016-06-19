// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "GameObjectBlueprintFactory.generated.h"

UCLASS()
class UGameObjectBlueprintFactory : public UFactory
{
	GENERATED_BODY()
public:

	UGameObjectBlueprintFactory(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category="GameObjectBlueprintFactory")
	TSubclassOf<class UGameObject> ParentClass;

	// UFactory interface
	virtual uint32 GetMenuCategories() const;
	virtual FText GetDisplayName() const override;
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;	
	// End of UFactory interface
};