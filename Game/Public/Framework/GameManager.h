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

UCLASS(BlueprintType)
class GAME_API USaveGameBase : public USaveGame
{
    GENERATED_BODY()
public:

    UPROPERTY()
	FGameRecord GameRecord;
	
	// TBD: World saving
    // UPROPERTY()
	// FWorldRecord WorldRecord;
    
};

UCLASS(Blueprintable)
class GAME_API UGameManager : public UGameInstance
{
    GENERATED_BODY()
public:

	static UGameManager* Get();
	static UGameManager* GetChecked();

public:

	UGameManager( const FObjectInitializer& ObjectInitializer );

	UFUNCTION( BlueprintPure, Category = "GameManager" )
	UGame* GetCurrentGame() const;

	UFUNCTION(BlueprintCallable, Category="GameManager")
	UGame* StartGame();

	UFUNCTION(BlueprintCallable, Category="GameManager")
	void StopGame();

	UFUNCTION(BlueprintCallable, Category="GameManager")
	bool LoadGame( const FString& SlotName, int32 UserIndex = 0 );

	UFUNCTION(BlueprintCallable, Category="GameManager")
	bool SaveGame( const FString& SlotName, int32 UserIndex = 0 );

	UFUNCTION(BlueprintPure, Category="GameSystem")
	USoundBank* GetSoundBank( const FString& ID ) const;

	UFUNCTION(BlueprintPure, Category = "GameManager")
	void GetBlueprintGeneratedClassReferences( UClass* ParentClass, TArray<FStringClassReference>& OutClassReferences ) const;

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

	UPROPERTY( Transient )
	UGame* CurrentGame;

	UPROPERTY()
	TMap<FString, TAssetPtr<USoundBank>> SoundBanks;

	// TBD: Blueprint generated class collection will be automatically populated by GameEd.
	UPROPERTY(VisibleDefaultsOnly, AdvancedDisplay, Category="AssetCollection")
	TArray<FBlueprintGeneratedClassCollection> BlueprintGeneratedClassCollections;

	/** Called before the game is saved to set custom save game object properties 
	* @Note: not very useful if you're not using custom save game class. 
	*/
	virtual void PreSaveGame( USaveGameBase* SaveGame ) {}

	/** Called after the game has been saved to restore custom save game object properties
	* @Note: not very useful if you're not using custom save game class. 
	*/
	virtual void PostLoadGame( USaveGameBase* SaveGame ) {}

	UFUNCTION(BlueprintImplementableEvent, Category="GameManager", meta=(DisplayName="PreSaveGame"))
	void ReceivePreSaveGame( USaveGameBase* SaveGame );

	UFUNCTION(BlueprintImplementableEvent, Category="GameManager", meta=(DisplayName="PostSaveGame"))
	void ReceivePostLoadGame( USaveGameBase* SaveGame );

};
