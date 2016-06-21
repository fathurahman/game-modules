// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameTypes.h"
#include "Object/GameData.h"
#include "Object/GameObjectTree.h"
#include "Framework/SoundBank.h"
#include "Util/BlueprintClassRegistry.h"

#include "GameStatics.generated.h"

class UAudioComponent;

extern GAME_API FRandomStream GRandomStream;

UCLASS()
class GAME_API UGameStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/** Get the Game Manager object. */
	UFUNCTION(BlueprintPure, Category="GameStatics")
	static class UGameManager* GetGameManager();

	/** Get the current game. This function will return null if there is no running game. */
	UFUNCTION(BlueprintPure, Category="GameStatics")
	static UGame* GetGame();

	/** Get the current game, if it does not exist then starts a new one. */
	UFUNCTION(BlueprintPure, Category="GameStatics")
	static UGame* GetGameSafe();

	/** Get the per-game random stream. */
	UFUNCTION(BlueprintPure, Category="GameStatics")
	static void GetRandomStream( FRandomStream& OutRandomStream ) { OutRandomStream = GRandomStream; }
	
	UFUNCTION(BlueprintPure, Category="Tag")
	static void GetObjectsWithTag( FString InTag, TSubclassOf<UObjectWithTags> Class, TArray<UObjectWithTags*>& OutObjects );

	UFUNCTION(BlueprintPure, Category="Tag")
	static void GetObjectsWithTags( ETagFilterType FilterType, TArray<FString> InTags, TSubclassOf<UObjectWithTags> Class, TArray<UObjectWithTags*>& OutObjects );

	UFUNCTION(BlueprintPure, Category="Tag", meta=(WorldContext="WCO"))
	static void GetActorsWithTag( UObject* WCO, FString InTag, TArray<AActor*>& OutActors );

	UFUNCTION(BlueprintPure, Category="Tag", meta=(WorldContext="WCO"))
	static void GetActorsWithTags( UObject* WCO, ETagFilterType FilterType, TArray<FString> InTags, TArray<AActor*>& OutActors );

	UFUNCTION(BlueprintCallable, Category="GameData")
	static void GetGameDataByClass(TSubclassOf<UGameData> Class, TArray<UGameData*>& OutList);

	UFUNCTION(BlueprintCallable, Category="GameObject")
	static void GetGameObjectClasses(TSubclassOf<UGameObject> Class, TArray<UClass*>& OutClasses);

	UFUNCTION(BlueprintPure, Category="GameStatics")
	static USoundBank* GetSoundBank( USoundBank* SoundBank );

	UFUNCTION(BlueprintCallable, Category="GameStatics", meta=(DisplayName="PlaySoundBankEntry"))
	static bool BPF_PlaySoundBankEntry( USoundBank* SoundBank, FString SoundID );

	UFUNCTION(BlueprintCallable, Category="GameStatics", meta=(DisplayName="PlaySoundBankEntryAtLocation", AdvancedDisplay="3"))
	static bool BPF_PlaySoundBankEntryAtLocation( USoundBank* SoundBank, FString SoundID, FVector Location, FRotator Rotation = FRotator::ZeroRotator );

	UFUNCTION(BlueprintCallable, Category="GameStatics", meta=(DisplayName="PlaySoundBankEntryAttached", AdvancedDisplay = "3"))
	static UAudioComponent* BPF_PlaySoundBankEntryAttached( USoundBank* SoundBank, FString SoundID, class USceneComponent* AttachToComponent, FName AttachPointName = NAME_None, FVector Location = FVector(ForceInit), FRotator Rotation = FRotator::ZeroRotator, EAttachLocation::Type LocationType = EAttachLocation::KeepRelativeOffset, bool bStopWhenAttachedToDestroyed = false);

	UFUNCTION(BlueprintCallable, Category="GameStatics", meta=(ExpandEnumAsExecs="OutResult", DisplayName="LoadTexture2DFromFile"))
	static UTexture2D* BPF_LoadTexture2DFromFile(const FString& FullFilePath, EImageFileFormat ImageFileFormat, int32& OutWidth, int32& OutHeight, ESuccess& OutResult);

	UFUNCTION(BlueprintPure, Category="GameStatics", meta=(DisplayName="GetBlueprintClassRegistry"))
	static UBlueprintClassRegistry* BPF_GetBlueprintClassRegistry( UClass* ParentClass );
};