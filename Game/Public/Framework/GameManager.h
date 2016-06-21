// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "Engine/GameInstance.h"
#include "GameTypes.h"
#include "SoundBank.h"
#include "Game.h"
#include "GameManager.generated.h"

USTRUCT()
struct FBlueprintGeneratedClassCollection
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FStringClassReference ParentClass;

	UPROPERTY()
	TArray<FStringClassReference> GeneratedClasses;
    
};

/**
* Save Game Object.
* It can be used as-is, or it can be derived to add more properties to the save game.
* Derived class should be specified as the UGameManager::SaveGameClass property
* Additional properties in a derived class should be set during UGameManager::PreSaveGame and retrieved during UGameManager::PostLoadGame
*/
UCLASS(BlueprintType)
class GAME_API USaveGameObject : public USaveGame
{
    GENERATED_BODY()
public:

    UPROPERTY()
	FGameRecord GameRecord;
	
	// TBD: World saving
    // UPROPERTY()
	// FWorldRecord WorldRecord;
    
};

/**
* Game Manager
* This is a game instance, which means it's always exist when the game is running (but not in the editor),
* it manages game (among many things).
*/
UCLASS(Blueprintable)
class GAME_API UGameManager : public UGameInstance
{
    GENERATED_BODY()
public:

	static UGameManager* Get();
	static UGameManager* GetChecked();

public:

	UGameManager( const FObjectInitializer& ObjectInitializer );

	/** Get the currently running game. */
	UFUNCTION( BlueprintPure, Category = "GameManager" )
	UGame* GetCurrentGame() const;

	/** Start a new game. */
	UFUNCTION(BlueprintCallable, Category="GameManager")
	UGame* StartGame();

	/** Stop the currently running game. */
	UFUNCTION(BlueprintCallable, Category="GameManager")
	void StopGame();

	/** Load an existing game. */
	UFUNCTION(BlueprintCallable, Category="GameManager")
	bool LoadGame( const FString& SlotName, int32 UserIndex = 0 );

	/** Save an existing game. */
	UFUNCTION(BlueprintCallable, Category="GameManager")
	bool SaveGame( const FString& SlotName, int32 UserIndex = 0 );

	// TODO: move this to GameDataManager
	UFUNCTION(BlueprintPure, Category="GameSystem")
	USoundBank* GetSoundBank( const FString& ID ) const;

	// TODO: move this to GameDataManager
	UFUNCTION(BlueprintPure, Category = "GameManager")
	void GetBlueprintGeneratedClassReferences( UClass* ParentClass, TArray<FStringClassReference>& OutClassReferences ) const;

	// TODO: move this to GameDataManager
	UFUNCTION(BlueprintPure, Category="GameManager")
	void GetBlueprintGeneratedClasses( UClass* ParentClass, TArray<UClass*>& OutClasses ) const;

	// UGameInstance interface
	virtual void Init() override;
	virtual void Shutdown() override;
	// End of UGameInstance interface

protected:

	UPROPERTY( EditAnywhere, Category = "Classes", meta=(MetaClass="Game", AllowAbstract="False") )
	FStringClassReference GameClass;

	UPROPERTY(EditAnywhere, NoClear, Category="GameManager", meta=(MetaClass="SaveGameBase", AllowAbstract="False"))
	FStringClassReference SaveGameClass;

	/** The current running game. */
	UPROPERTY( Transient )
	UGame* CurrentGame;

	/** Called before the game is saved to set custom save game object properties 
	* @Note: not very useful if you're not using custom save game class. 
	*/
	virtual void PreSaveGame( USaveGameObject* SaveGame ) {}

	/** Called after the game has been saved to restore custom save game object properties
	* @Note: not very useful if you're not using custom save game class. 
	*/
	virtual void PostLoadGame( USaveGameObject* SaveGame ) {}

	/** Event triggered before the game is saved to a save slot. */
	UFUNCTION(BlueprintImplementableEvent, Category="GameManager", meta=(DisplayName="PreSaveGame"))
	void BPF_PreSaveGame( USaveGameObject* SaveGame );

	/** Event triggered after the game has just been loaded from a save slot. */
	UFUNCTION(BlueprintImplementableEvent, Category="GameManager", meta=(DisplayName="PostLoadGame"))
	void BPF_PostLoadGame( USaveGameObject* SaveGame );

	/** TODO: move this to GameDataManager. */
	UPROPERTY()
	TMap<FString, TAssetPtr<USoundBank>> SoundBanks;

	// TBD: Blueprint generated class collection will be automatically populated by GameEd.
	// TODO: move this to GameDataManager.
	UPROPERTY(VisibleDefaultsOnly, AdvancedDisplay, Category="AssetCollection")
	TArray<FBlueprintGeneratedClassCollection> BlueprintGeneratedClassCollections;

};
