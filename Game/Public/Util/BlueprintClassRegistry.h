#pragma once

#include "Object/GameData.h"
#include "BlueprintClassRegistry.generated.h"

USTRUCT()
struct GAME_API FBlueprintClassRegistryEntry
{
    GENERATED_USTRUCT_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BlueprintClassRegistry")
	FStringClassReference ParentClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BlueprintClassRegistry")
	TArray<FStringClassReference> BlueprintClasses;

    
};

UCLASS(BlueprintType)
class GAME_API UBlueprintClassRegistry : public UGameData
{
    GENERATED_BODY()
public:

	UBlueprintClassRegistry(const FObjectInitializer& ObjectInitializer);		

	UFUNCTION(BlueprintPure, Category="BlueprintClassRegistry")
	UClass* GetParentClass() const;

	UFUNCTION(BlueprintPure, Category="BlueprintClassRegistry")
	void GetBlueprintClasses( TArray<UClass*>& OutBlueprintClasses );

	// UObject interface
	virtual void PostInitProperties() override;
#if WITH_EDITOR
	virtual bool CanEditChange( const UProperty* InProperty ) const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	// End of UObject interface
	
protected:

	UPROPERTY(EditInstanceOnly, Category="Parent", meta=(MetaClass="Object", AllowAbstract="True"))
	FStringClassReference ParentClass;


#if WITH_EDITORONLY_DATA

	UPROPERTY(EditInstanceOnly, Category="Populate")
	bool bRecursiveClasses = false;

	UPROPERTY(EditInstanceOnly, Category="Populate")
	TArray<FName> SearchPaths;

	/** Click this flag if there's a problem with class registration, this is as fail-safe measure that should not have been needed. */
	UPROPERTY(Transient, EditInstanceOnly, Category="Populate")
	bool bPopulateBlueprintClasses = false;

#endif

	UPROPERTY(EditAnywhere, Category="Registry", meta=(MetaClass="Object"))
	TArray<FStringClassReference> BlueprintClasses;


#if WITH_EDITOR

	void PopulateBlueprintClasses();
	void OnInMemoryAssetCreated(UObject* Object);
	void OnInMemoryAssetDeleted(UObject* Object);

#endif

};
