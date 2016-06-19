// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GamePrivatePCH.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "Framework/SoundBank.h"

USoundBank::USoundBank( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
}

void USoundBank::SetGameInstance( UGameInstance* InGameInstance )
{
	GameInstance = InGameInstance;
}

bool USoundBank::HasSoundBankEntry( const FString& ID ) const
{
	return GetSoundBankEntryPtr( ID ) != nullptr;
}

void USoundBank::GetSoundBankEntry(const FString& ID, FSoundBankEntry& OutSoundEffect, EValidity& OutValidity) const
{
	const FSoundBankEntry* pSoundEffect = GetSoundBankEntryPtr( ID );
	if ( pSoundEffect )
	{
		OutSoundEffect = *pSoundEffect;
		OutValidity = EValidity::Valid;
	}
	else
	{
		OutValidity = EValidity::NotValid;
	}
}

bool USoundBank::Play( const FString& ID, UWorld* World ) const
{
	if ( !World )
	{
		World = GameInstance.IsValid() ? GameInstance->GetWorld() : nullptr;
		if ( !World )
		{
			return nullptr;
		}
	}

	const FSoundBankEntry* SoundEffect = GetSoundBankEntryPtr( ID );
	if ( !SoundEffect )
	{
		PrintLogWarning("There is no sound effect with ID [%s] in %s", *ID, *GetName());
		return false;
	}

	USoundBase* Sound = GetOrLoadAssetFromPtr( SoundEffect->Sound );
	if ( !Sound )
	{
		PrintLogWarning("SoundEffect [%s] Sound is not valid: %s", *ID, *SoundEffect->Sound.ToString() );
		return false;
	}

	USoundConcurrency* SoundConcurrency = GetOrLoadAssetFromPtr( SoundEffect->SoundConcurrency );

	UGameplayStatics::PlaySound2D(
		World, 
		Sound, 
		SoundEffect->VolumeMultiplier, 
		SoundEffect->PitchMultiplier, 
		SoundEffect->StartTime,
		SoundConcurrency);
	return true;
}


bool USoundBank::PlayAtLocation(const FString& ID, const FVector& Location, const FRotator& Rotation, UWorld* World) const
{
	if ( !World )
	{
		World = GameInstance.IsValid() ? GameInstance->GetWorld() : nullptr;
		if ( !World )
		{
			return nullptr;
		}
	}

	const FSoundBankEntry* SoundEffect = GetSoundBankEntryPtr( ID );
	if ( !SoundEffect )
	{
		PrintLogWarning("There is no sound effect with ID [%s] in %s", *ID, *GetName());
		return false;
	}

	USoundBase* Sound = GetOrLoadAssetFromPtr( SoundEffect->Sound );
	if ( !Sound )
	{
		PrintLogWarning("SoundEffect [%s] Sound is not valid: %s", *ID, *SoundEffect->Sound.ToString() );
		return false;
	}

	USoundConcurrency* SoundConcurrency = GetOrLoadAssetFromPtr( SoundEffect->SoundConcurrency );

	USoundAttenuation* SoundAttenuation = GetOrLoadAssetFromPtr( SoundEffect->SoundAttenuation );

	UGameplayStatics::PlaySoundAtLocation(
		World,
		Sound,
		Location,
		Rotation,
		SoundEffect->VolumeMultiplier,
		SoundEffect->PitchMultiplier,
		SoundEffect->StartTime,
		SoundAttenuation,
		SoundConcurrency);
	return true;
}



UAudioComponent* USoundBank::PlayAttached(const FString& ID, USceneComponent* AttachToComponent, FName AttachPointName, const FVector& Location, const FRotator& Rotation, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroyed) const
{
	const FSoundBankEntry* SoundEffect = GetSoundBankEntryPtr( ID );
	if ( !SoundEffect )
	{
		PrintLogWarning("There is no sound effect with ID [%s] in %s", *ID, *GetName());
		return nullptr;
	}

	USoundBase* Sound = GetOrLoadAssetFromPtr( SoundEffect->Sound );
	if ( !Sound )
	{
		PrintLogWarning("SoundEffect [%s] Sound is not valid: %s", *ID, *SoundEffect->Sound.ToString() );
		return nullptr;
	}

	USoundConcurrency* SoundConcurrency = GetOrLoadAssetFromPtr( SoundEffect->SoundConcurrency );

	USoundAttenuation* SoundAttenuation = GetOrLoadAssetFromPtr( SoundEffect->SoundAttenuation );

	return UGameplayStatics::SpawnSoundAttached(
		Sound,
		AttachToComponent,
		AttachPointName,
		Location,
		Rotation,
		LocationType,
		bStopWhenAttachedToDestroyed,
		SoundEffect->VolumeMultiplier,
		SoundEffect->PitchMultiplier,
		SoundEffect->StartTime,
		SoundAttenuation,
		SoundConcurrency);
}

UWorld* USoundBank::GetWorld() const
{
	return GameInstance.IsValid() ? GameInstance->GetWorld() : nullptr;
}

void USoundBank::PostInitProperties()
{
	Super::PostInitProperties();
	if ( IsAsset() )
	{
		if ( SoundBankID.IsEmpty() )
		{
			GetName( SoundBankID );
		}
	}
}

const FSoundBankEntry* USoundBank::GetSoundBankEntryPtr(const FString& ID) const
{
	if ( SoundBankEntryMap.Num() == 0 )
	{
		if ( SoundBankEntries.Num() == 0 )
		{
			return nullptr;
		}
		else
		{
			FSoundBankEntryMap* Map = (FSoundBankEntryMap*)(&SoundBankEntryMap);
			for ( const FSoundBankEntry& SoundEffect : SoundBankEntries )
			{
				Map->Add( SoundEffect.ID, &SoundEffect );
			}
		}
	}

	const FSoundBankEntry** ppSoundEffect = SoundBankEntryMap.Find( ID );
	if ( ppSoundEffect )
	{
		return *ppSoundEffect;
	}

	return nullptr;
}
