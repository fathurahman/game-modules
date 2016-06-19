// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GamePrivatePCH.h"
#include "Util/GameUtil.h"
#include "Framework/GameManager.h"

UGame* UGame::Get()
{
	UGameManager* GameManager = UGameManager::Get();
	return GameManager ? GameManager->GetCurrentGame() : nullptr;
}

UGame* UGame::GetOrStart()
{	
	UGameManager* GameManager = UGameManager::Get();
	check(GameManager);

	UGame* Game = GameManager->GetCurrentGame();
	if ( !Game )
	{
		Game = GameManager->StartGame();
	}

	return Game;
}

UGame::UGame( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	ObjectTree = ObjectInitializer.CreateDefaultSubobject<UGameObjectTree>( this, "ObjectRootDefault" );
}

void UGame::Init()
{
	OnInit();
}

void UGame::Start()
{
	SimulationDateTime = InitialSimulationDateTime;
	SimulationSpeedScale = 1.f;
	bIsSimulationPaused = false;

	OnStart();
}

#define GameMapToList(Map, List, Type) {\
	List.Empty(); \
	for ( const auto& i : Map ) { \
		List.Add( Type( i.Key, i.Value ) ); \
	} \
}

#define GameListToMap(Map, List, Type) {\
	Map.Empty(); \
	for (const auto& i : List) { \
		Map.Add( i.Name, i.Value ); \
	} \
}

bool UGame::LoadFromRecord(const FGameRecord& InRecord)
{
	if ( ObjectTree->LoadFromRecord( InRecord.ObjectTreeRecord ) == false )
	{
		return false;
	}

	GameListToMap( GameFlags, InRecord.GameFlags, FNamedBoolean );
	GameListToMap( GameInts, InRecord.GameInts, FNamedInteger );
	GameListToMap( GameFloats, InRecord.GameFloats, FNamedFloat );
	GameListToMap( GameStrings, InRecord.GameStrings, FNamedString );

	FMemoryReader MemReader( InRecord.ByteData );
	FSaveGameArchive Ar( MemReader );
	this->Serialize( Ar );

	return true;
}

bool UGame::SaveToRecord(FGameRecord& OutRecord) const
{
	if ( ObjectTree->SaveToRecord( OutRecord.ObjectTreeRecord ) == false )
	{
		return false;
	}

	GameMapToList( GameFlags, OutRecord.GameFlags, FNamedBoolean );
	GameMapToList( GameInts, OutRecord.GameInts, FNamedInteger );
	GameMapToList( GameFloats, OutRecord.GameFloats, FNamedFloat );
	GameMapToList( GameStrings, OutRecord.GameStrings, FNamedString );

	FMemoryWriter MemWriter( OutRecord.ByteData );
	FSaveGameArchive Ar( MemWriter );
	UObject* Self = (UObject*) this;
	Self->Serialize( Ar );

	return true;
}

void UGame::Dispose()
{
	if ( IsPendingKill() )
	{
		return;
	}

	OnDispose();

	ObjectTree->Dispose();

	MarkPendingKill();
}

UGameObjectTree* UGame::GetObjectTree() const
{
	return nullptr;
	//return ObjectRoot;
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
	/*
	ObjectRoot->Tick( DeltaTime );

	if ( bHasSimulation && bIsSimulationPaused == false )
	{
		if ( bIsSimulationAffectsWorldTime )
		{
			FTimespan Timespan = SimulationTimespanPerSeconds * DeltaTime;
			SimulationDateTime += Timespan;
			ObjectRoot->SimulationTick( Timespan );
		}
		else
		{
			FTimespan Timespan = SimulationTimespanPerSeconds * (DeltaTime * SimulationSpeedScale);
			SimulationDateTime += Timespan;
			ObjectRoot->SimulationTick( Timespan );
		}
	}
	*/
}


bool UGame::GetGameFlag( FString Name, bool bDefaultValue ) const
{
	return GetGameMapValue( GameFlags, Name, bDefaultValue );
}


void UGame::SetGameFlag(FString Name, bool bValue)
{
	SetGameMapValue( GameFlags, Name, bValue );
}


int32 UGame::GetGameInt(FString Name, int32 DefaultValue) const
{
	return GetGameMapValue( GameInts, Name, DefaultValue );
}

void UGame::SetGameInt(FString Name, int32 Value)
{
	SetGameMapValue( GameInts, Name, Value );
}

float UGame::GetGameFloat(FString Name, float DefaultValue) const
{
	return GetGameMapValue( GameFloats, Name, DefaultValue );
}

void UGame::SetGameFloat(FString Name, float Value)
{
	SetGameMapValue( GameFloats, Name, Value );
}


FString UGame::GetGameString(FString Name, FString DefaultValue) const
{
	return GetGameMapValue( GameStrings, Name, DefaultValue );
}

void UGame::SetGameString(FString Name, FString Value)
{
	SetGameMapValue( GameStrings, Name, Value );
}

UWorld* UGame::GetWorld() const
{
	UGameManager* GameManager = UGameManager::Get();
	return GameManager ? GameManager->GetWorld() : nullptr;
}

