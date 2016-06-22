#pragma once

#include "GameDataManager.generated.h"

/** Game Data Manager.
* This class (or its derivative) must be set as the game singleton for the project.
*/
UCLASS(Blueprintable)
class GAME_API UGameDataManager : public UObject
{
    GENERATED_BODY()
public:

	friend class UGameManager;

	static UGameDataManager* Get();

	UGameDataManager( const FObjectInitializer& ObjectInitializer );

	virtual void PostInitProperties() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

		
protected:

	//////////////////////////////////////////////////////////////////////////
	//
	// Game Data asset collection
	//
	// By default all UGameData assets are automatically collected by UGameDataManager,
	// but you can also disable the auto data collector and fill in the data yourself,
	// or you can manually specify which folders to cook in the packaging settings and make
	// sure all the data that you need are cooked and packaged.
	//
	// The reason we need this mechanism is to make Unreal Engine packager cook and package our
	// Game Data that are not referenced by actor in the existing maps, let's say for example 
	// we create an item data that will only be referenced by GameObject which is dynamically 
	// created at a certain time in the game, without this collection (or without manually
	// specifying the folders in which they reside) then those data will not be cooked and packaged
	// by Unreal Engine.
	//
	// Because of the purpose of this mechanic, the collection is cleaned up when the game starts
	// to slightly reduce memory usage. You can use GetAssetPtrs or its variants to get the actual
	// reference to game data assets that you've made.
	//
	//////////////////////////////////////////////////////////////////////////

#if WITH_EDITORONLY_DATA
	
	UPROPERTY(EditDefaultsOnly, Category="GameDataCollection", meta=(DisplayName="AutoCollect"))
	bool bUseGameDataAssetCollector = true;
	
	/** Specify which folders to be included by the game data collector, 
	* If these array is empty then the collector will include any folders except
	* the ones the excluded ones ( @see GameDataExcludePaths )
	*/
	UPROPERTY(EditDefaultsOnly, Category="GameDataCollection", meta=(DisplayName="IncludePaths", EditCondition="bUseGameDataAssetCollector"))
	TArray<FString> GameDataAssetIncludePaths;

	/** Specify which folders to be excluded by the game data collector.
	* If these array is empty then the collector will not exclude any folders.
	*/
	UPROPERTY(EditDefaultsOnly, Category="GameDataCollection", meta=(DisplayName="ExcludePaths", EditCondition="bUseGameDataAssetCollector"))
	TArray<FString> GameDataAssetExcludePaths;

	UPROPERTY(EditDefaultsOnly, Category="GameDataCollection", meta=(DisplayName="Recollect", EditCondition="bUseGameDataAssetCollector"))
	bool bRecollectGameDataAssets = false;

#endif // WITH_EDITORONLY_DATA

	/** List of game data asset references. */
	UPROPERTY(EditDefaultsOnly, Category="GameDataCollection", meta=(EditCondition="!bUseGameDataCollector", AllowedClasses="GameData", ExactClass="False"))
	TArray<FStringAssetReference> GameDataAssetReferences;

	//////////////////////////////////////////////////////////////////////////
	//
	// Blueprint Generated Classes Collection
	//
	// The reason for this is the same as Game Data Assets Collections, in order
	// to make Unreal Engine packager compile the blueprint and package its generated classes.
	//
	// GameObject blueprints will always be collected because the nature of GameObject
	// that is not being referenced in the map. But you can also specify your own 
	// blueprints by specifying parent classes of the blueprint to be collected.
	//
	//////////////////////////////////////////////////////////////////////////

#if WITH_EDITORONLY_DATA

	UPROPERTY(EditDefaultsOnly, Category="GeneratedClassCollection", meta=(DisplayName="AutoCollect"))
	bool bUseGeneratedClassCollector = true;

	UPROPERTY(EditDefaultsOnly, Category="GeneratedClassCollection", meta=(DisplayName="IncludePaths", EditCondition="bUseGeneratedClassCollector"))
	TArray<FString> GeneratedClassIncludePaths;

	UPROPERTY(EditDefaultsOnly, Category="GeneratedClassCollection", meta=(DisplayName="ExludePaths", EditCondition="bUseGeneratedClassCollector"))
	TArray<FString> GeneratedClassExcludePaths;

	UPROPERTY(EditDefaultsOnly, Category="GeneratedClassCollection", meta=(DisplayName="ParentClasses", MetaClass="Object", AllowAbstract="True", EditCondition="bUseGeneratedClassCollector"))
	TArray<FStringClassReference> AdditionalParentClassReferences;

	UPROPERTY(EditDefaultsOnly, Category="GeneratedClassCollection", meta=(DisplayName="Recollect", EditCondition="bUseGeneratedClassCollector"))
	bool bRecollectGeneratedClasses = false;

#endif // WITH_EDITORONLY_DATA

	UPROPERTY(EditDefaultsOnly, Category="GeneratedClassCollection", meta=(MetaClass="BlueprintGeneratedClass", AllowAbstract="False"))
	TArray<FStringClassReference> GeneratedClassReferences;

private:

#if WITH_EDITOR

	void RecollectGameDataAssetReferences();

	void RecollectGeneratedClassReferences();

	void OnInMemoryAssetCreated( UObject* Asset );

	void OnInMemoryAssetDeleted( UObject* Asset );

#endif
    
};

