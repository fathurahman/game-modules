// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "GameTypes.h"
#include "Object/GameData.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundConcurrency.h"
#include "Components/AudioComponent.h"
#include "Components/SceneComponent.h"
#include "SoundBank.generated.h"

USTRUCT()
struct GAME_API FSoundBankEntry
{
    GENERATED_USTRUCT_BODY()

	/** Identifier string. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SoundEffect")
	FString ID;
    
	/** The sound asset. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SoundEffect")
	TAssetPtr<USoundBase> Sound;

	/** Volume Multiplier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SoundEffect", meta=(UIMin="0"))
	float VolumeMultiplier = 1.f;

	/** Pitch Multiplier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SoundEffect", meta=(UIMin="0"))
	float PitchMultiplier = 1.f;

	/** StartTime - How far in to the sound to begin playback at. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SoundEffect")
	float StartTime = 0.f;

	/** AttenuationSettings - Override attenuation settings package to play sound with. */
	UPROPERTY(EditAnywhere, Category="SoundEffect")
	TAssetPtr<USoundAttenuation> SoundAttenuation;

	/** ConcurrencySettings - Override concurrency settings package to play sound with. */
	UPROPERTY(EditAnywhere, Category="SoundEffect")
	TAssetPtr<USoundConcurrency> SoundConcurrency;
    
};


/**
* Helper class to easily manage and play sound effects. 
* This class is managed by game instance (@see UGameInstanceBase).
* Note: Depending on the number of sound effects you have in the game, you may want to make several of SoundEffectBank
*		based on the sound effect category
*/
UCLASS(BlueprintType)
class GAME_API USoundBank : public UDataAsset
{
    GENERATED_BODY()
public:

	/** Constructor. */
	USoundBank( const FObjectInitializer& ObjectInitializer );

	/** Set the game instance in which this sound bank is managed. */
	void SetGameInstance( UGameInstance* InGameInstance );

	/** Get this sound bank's ID */
	const FString& GetID() const { return SoundBankID; }

	/**
	* Check whether this sound bank has a specific sound ID
	* @param	ID		The sound bank entry's ID
	* @return false if there's no such ID.
	*/
	UFUNCTION(BlueprintPure, Category="SoundBank")
	bool HasSoundBankEntry( UPARAM(Ref) const FString& ID ) const;

	/**
	* Get sound bank entry with a certain ID.
	* @param		ID					The sound bank entry's ID.
	* @param[out]	OutSoundBankEntry	The sound entry
	* @param[out]	OutValidity			Whether the sound entry is valid or not.
	*/
	UFUNCTION(BlueprintPure, Category="SoundBank", meta=(ExpandEnumAsExecs="OutValidity"))
	void GetSoundBankEntry( const FString& ID, FSoundBankEntry& OutSoundBankEntry, EValidity& OutValidity ) const;

	/**
	* Get the sound bank entry pointer. It's a native version of GetSoundBankEntry
	* @param ID	The sound bank entry's ID.
	* @return nullptr if there's no such ID.
	*/
	const FSoundBankEntry* GetSoundBankEntryPtr( const FString& ID ) const;

	/**
	* Play a 2D sound bank entry. (Not Replicable)
	* This is Fire and forget playback, you play the sound and you just forget about it. The audio component will destroy itself when it's finished.
	* @note an easier way to do this is using the global function @PlaySoundBankEntry (@see GameUtil.h)
	* @param	ID		The sound bank entry's ID.
	* @param	World	(Optional) The world where the sound will be played in, if null then the current world is used instead.
	* @return false if the sound can not be played.
	*/
	bool Play( const FString& ID, UWorld* World = nullptr ) const;

	/**
	* Play a sound bank entry at a certain location in the world. (Replicable)
	* @note	an easier way to do this is using the global function @PlaySoundBankEntryAtLocation (@see GameUtil.h)
	* @param	ID			The sound bank entry's ID.
	* @param	Location	The location in which the sound will be played in.
	* @param	Rotation	The sound's rotation
	* @param	World		(Optional) THe world where the sound will be played in, if null then the current world is used instead.
	* @return false if the sound can not be played.
	*/
	bool PlayAtLocation( const FString& ID, const FVector& Location, const FRotator& Rotation = FRotator::ZeroRotator, UWorld* World = nullptr ) const;

	/**
	* Play a sound bank entry that is attached to a certain scene component.
	* @note an easier way to do this is using the global function @PlaySoundBankEntryAttached
	* @param	ID								The sound bank entry's ID.
	* @param	AttachToComponent				The component where the sound will be attached to
	* @param	AttachPointName					The slot where the sound will be attached to
	* @param	Location						Attachment location
	* @param	Rotation						Attachment rotation
	* @param	LocationType					Attachment location's type
	* @param	bStopWhenAttachedToDestroyed	Whether to stop the sound when the component it is attached to is destroyed.
	* @return nullptr if the attachment failed.
	*/
	UAudioComponent* PlayAttached( const FString& ID, USceneComponent* AttachToComponent, FName AttachPointName = NAME_None, const FVector& Location = FVector::ZeroVector, const FRotator& Rotation = FRotator::ZeroRotator, EAttachLocation::Type LocationType = EAttachLocation::KeepRelativeOffset, bool bStopWhenAttachedToDestroyed = false) const;

	// UObject interface
	virtual UWorld* GetWorld() const override;
	virtual void PostInitProperties() override;
	// End of UObject interface

protected:

	UPROPERTY(EditAnywhere, Category="SoundEffectBank")
	FString SoundBankID;

	UPROPERTY(EditAnywhere, Category="SoundEffectBank")
	TArray<FSoundBankEntry> SoundBankEntries;


private:

	typedef TMap<FString, const FSoundBankEntry*> FSoundBankEntryMap;

	mutable FSoundBankEntryMap SoundBankEntryMap;

	TWeakObjectPtr<UGameInstance> GameInstance;


};