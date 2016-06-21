// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GamePrivatePCH.h"
#include "Util/GameUtil.h"
#include "Framework/GameManager.h"

UGame* UGame::Get( bool bChecked )
{
	UGameManager* GameManager = UGameManager::Get();
	check( !bChecked || GameManager );

	UGame* Game = GameManager->GetCurrentGame();
	check( !bChecked || Game );

	return Game;
}

UGame* UGame::GetSafe()
{	
	UGameManager* GameManager = UGameManager::Get();
	check( GameManager );

	UGame* Game = GameManager->GetCurrentGame();
	if ( !Game )
	{
		Game = GameManager->StartGame();
	}

	return Game;
}

UGame::UGame( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
	, bIsInitialized( false )
{
	ObjectTree = ObjectInitializer.CreateDefaultSubobject<UGameObjectTree>( this, "ObjectRootDefault" );
}

void UGame::Init()
{
	ensureMsgf( !bIsInitialized, TEXT("Trying to initialize an already initialized game.") );

	bIsInitialized = true;

	OnInit();
	BPF_OnInit();
}

void UGame::Start()
{
	Init();
	SimulationDateTime = InitialSimulationDateTime;
	OnStart();
	BPF_OnStart();
}

bool UGame::LoadFromRecord(const FGameRecord& InRecord)
{
	Init();

	if ( ObjectTree->LoadFromRecord( InRecord.ObjectTreeRecord ) == false )
	{
		return false;
	}

	FMemoryReader MemReader( InRecord.ByteData );
	FSaveGameArchive Ar( MemReader );
	this->Serialize( Ar );

	OnLoadedFromRecord();
	BPF_OnLoadedFromRecord();

	return true;
}

bool UGame::SaveToRecord(FGameRecord& OutRecord) const
{
	ensureMsgf( bIsInitialized, TEXT( "Trying to save an un-initialized game to a record." ) );

	if ( ObjectTree->SaveToRecord( OutRecord.ObjectTreeRecord ) == false )
	{
		return false;
	}

	FMemoryWriter MemWriter( OutRecord.ByteData );
	FSaveGameArchive Ar( MemWriter );
	UObject* Self = (UObject*) this;
	Self->Serialize( Ar );

	return true;
}

void UGame::Shutdown()
{
	if ( IsPendingKill() )
	{
		return;
	}

	OnShutdown();

	ObjectTree->Dispose();

	MarkPendingKill();
}

UGameObjectTree* UGame::GetObjectTree() const
{
	return ObjectTree;
}

bool UGame::HasSimulation() const
{
	return bHasSimulation;
}

bool UGame::IsSimulationPaused() const
{
	return bIsSimulationPaused;
}

float UGame::GetSimulationSpeedScale() const
{
	return SimulationSpeedScale;
}

FDateTime UGame::GetSimulationDateTime() const
{
	return SimulationDateTime;
}

void UGame::SetIsSimulationPaused(bool bPaused)
{
	if ( bIsSimulationPaused != bPaused )
	{
		bIsSimulationPaused = bPaused;
		if ( bIsSimulationAffectsWorldTime )
		{
			UGameplayStatics::SetGamePaused( (UObject*) this, bPaused );
		}
	}
}

void UGame::SetSimulationSpeedScale(float SpeedScale)
{
	SpeedScale = FMath::Clamp( SpeedScale, 0.0001f, 20.f );
	if ( SimulationSpeedScale != SpeedScale )
	{		
		SimulationSpeedScale = SpeedScale;

		if ( bIsSimulationAffectsWorldTime )
		{
			UGameplayStatics::SetGlobalTimeDilation( (UObject*) this, SimulationSpeedScale );
		}
	}
}

void UGame::SetSimulationDateTime(const FDateTime& InDateTime)
{
	SimulationDateTime = InDateTime;
}

void UGame::Tick(float DeltaTime)
{
	ObjectTree->Tick( DeltaTime );

	if ( bHasSimulation && bIsSimulationPaused == false )
	{
		if ( bIsSimulationAffectsWorldTime )
		{
			FTimespan Timespan = SimulationTimespanPerSecond * DeltaTime;
			SimulationDateTime += Timespan;
			ObjectTree->SimulationTick( Timespan );
		}
		else
		{
			FTimespan Timespan = SimulationTimespanPerSecond * (DeltaTime * SimulationSpeedScale);
			SimulationDateTime += Timespan;
			ObjectTree->SimulationTick( Timespan );
		}
	}
}


bool UGame::GetGameFlag( FString Name, bool bDefaultValue ) const
{
	return GetNamedVar( GameFlags, Name, bDefaultValue );
}


void UGame::SetGameFlag(FString Name, bool bValue)
{
	SetNamedVar( GameFlags, Name, bValue );
}


int32 UGame::GetGameInt(FString Name, int32 DefaultValue) const
{
	return GetNamedVar( GameInts, Name, DefaultValue );
}

void UGame::SetGameInt(FString Name, int32 Value)
{
	SetNamedVar( GameInts, Name, Value );
}

float UGame::GetGameFloat(FString Name, float DefaultValue) const
{
	return GetNamedVar( GameFloats, Name, DefaultValue );
}

void UGame::SetGameFloat(FString Name, float Value)
{
	SetNamedVar( GameFloats, Name, Value );
}


FString UGame::GetGameString(FString Name, FString DefaultValue) const
{
	return GetNamedVar( GameStrings, Name, DefaultValue );
}

void UGame::SetGameString(FString Name, FString Value)
{
	SetNamedVar( GameStrings, Name, Value );
}

UWorld* UGame::GetWorld() const
{
	UGameManager* GameManager = UGameManager::Get();
	return GameManager ? GameManager->GetWorld() : nullptr;
}

