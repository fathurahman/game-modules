// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "Object/GameData.h"
#include "Object/GameObjectTree.h"
#include "Game.generated.h"


/** UGame
* A basic game Game class that stores object tree, can do simulation, and can be serialized to save game.
* @par
* Game is an object that should belong in a game instance and is persistent throughout the many game worlds. It should
* be created when the player 'starts' or 'loads' a game and destroyed when the players 'quit' (to main menu / system), there should be
* only one active Game in the game.
* @par
* This class can handle simulation (if you choose to enables it @see bAllowSimulation), it also has a default ObjectRoot in
* which you can add additional game object as its children. This class also support saving and loading of itself including
* the object root and its descendants from and to a save game record.
*/
UCLASS(Abstract, Blueprintable)
class GAME_API UGame : public UObject
{
	GENERATED_BODY()

public:

	//////////////////////////////////////////////////////////////////////////

	/** Get the currently running game. */
	static UGame* Get( bool bChecked = false );

	/** Get the currently running game. */
	//template<class T=UGame>
	//static FORCEINLINE T* Get( bool bChecked = false )
	//{
	//	return bChecked ? CastChecked<T>( Get(bChecked) ) :  Cast<T>( Get(bChecked) );
	//}

	/** Get the currently running game, if it does not exist then starts a new one. */
	static UGame* GetSafe();

	/** Get the currently running game, if it does not exist then starts a new one. */
	//template<class T=UGame>
	//static FORCEINLINE T* GetSafe()
	//{
	//	return Cast<T>( GetSafe() );
	//}

	//////////////////////////////////////////////////////////////////////////

	/** Constructor. */
	UGame( const FObjectInitializer& ObjectInitializer );

	/**
	* Starts a new Game.
	* Starts a 'New Game' and set Game properties accordingly.
	* @warning Either this function or @ref Load MUST be called before calling any other Game methods.
	*/
	UFUNCTION(BlueprintCallable, Category="Game")
	void Start();

	/**
	* Load Game from an existing record.
	* @warning Either this function or @ref Start MUST be called before calling any other Game methods.
	* @param	InRecord	The Game's record.
	*/
	UFUNCTION(BlueprintCallable, Category="Game")
	bool LoadFromRecord( const FGameRecord& InRecord );

	/** Save Game to a record.
	* @param[out]	OutRecord 	Record where this Game will be saved into.
	*/
	UFUNCTION(BlueprintCallable, Category="Game")
	bool SaveToRecord( FGameRecord& OutRecord ) const;

	/** Get the object tree root. */
	UFUNCTION(BlueprintPure, Category="GameObject")
	UGameObjectTree* GetObjectTree() const;	

	//////////////////////////////////////////////////////////////////////////
	// Simulation
	//////////////////////////////////////////////////////////////////////////

	/** Checks whether this Game has simulation or not. */
	UFUNCTION(BlueprintPure, Category="Simulation")
	bool HasSimulation() const;

	/** Check whether the simulation is currently paused or not. */
	UFUNCTION(BlueprintPure, Category="Simulation")
	bool IsSimulationPaused() const;

	/** Get the scale of the simulation speed. */
	UFUNCTION(BlueprintPure, Category="Simulation")
	float GetSimulationSpeedScale() const;

	/** Get the current simulation date time. */
	UFUNCTION(BlueprintPure, Category="Simulation")
	FDateTime GetSimulationDateTime() const;

	/** Set whether the simulation is paused or not. */
	UFUNCTION(BlueprintCallable, Category="Simulation")
	void SetIsSimulationPaused( bool bPaused );

	/** Set the scale of the simulation speed. */
	UFUNCTION(BlueprintCallable, Category="Simulation")
	void SetSimulationSpeedScale( float SpeedScale );

	/** Set the current simulation date time. */
	UFUNCTION(BlueprintCallable, Category="Simulation")
	void SetSimulationDateTime( const FDateTime& InDateTime );

	//////////////////////////////////////////////////////////////////////////
	// Named variables
	//////////////////////////////////////////////////////////////////////////

	/** Get the value of an arbitrary named boolean. */
	UFUNCTION(BlueprintPure, Category="Game")
	bool GetGameFlag( FString Name, bool bDefaultValue = false ) const;

	/** Set the value of an arbitrary named boolean. */
	UFUNCTION(BlueprintCallable, Category="Game")
	void SetGameFlag( FString Name, bool bValue );

	/** Get the value of an arbitrary named integer. */
	UFUNCTION(BlueprintPure, Category="Game")
	int32 GetGameInt( FString Name, int32 DefaultValue = 0 ) const;

	/** Set the value of an arbitrary named integer. */
	UFUNCTION(BlueprintCallable, Category="Game")
	void SetGameInt( FString Name, int32 Value );

	/** Get the value of an arbitrary named floating point. */
	UFUNCTION(BlueprintPure, Category="Game")
	float GetGameFloat( FString Name, float DefaultValue = 0.f ) const;

	/** Set the value of an arbitrary named floating point. */
	UFUNCTION(BlueprintCallable, Category="Game")
	void SetGameFloat( FString Name, float Value );

	/** Get the value of an arbitrary named string. */
	UFUNCTION(BlueprintPure, Category="Game")
	FString GetGameString( FString Name, FString DefaultValue = TEXT("") ) const;

	/** Set the value of an arbitrary named string. */
	UFUNCTION(BlueprintCallable, Category="Game")
	void SetGameString( FString Name, FString Value );

	//////////////////////////////////////////////////////////////////////////

	/** Tick this game. */
	UFUNCTION(BlueprintCallable, Category="Tick")
	void Tick( float DeltaTime );

	//////////////////////////////////////////////////////////////////////////

	/** Shutdown this Game and mark it as pending kill. 
	* @note It's preferred to force garbage collection after disposing a Game because it usually does takes a lot of memory. 
	*/
	void Shutdown();

	// UObject interface
	virtual UWorld* GetWorld() const override;
	// End of UObject interface.

protected:

	/** Whether this game has a simulation or not. */
	UPROPERTY(EditDefaultsOnly, Category="Simulation")
	bool bHasSimulation = false;

	/** The initial simulation date time when the game is started. */
	UPROPERTY(EditDefaultsOnly, Category="Simulation", meta=(EditCondition="bHasSimulation"))
	FDateTime InitialSimulationDateTime;

	/** How fast the simulation date time changes per second. */
	UPROPERTY(EditDefaultsOnly, Category="Simulation", meta=(EditCondition="bHasSimulation"))
	FTimespan SimulationTimespanPerSecond = FTimespan(0, 1, 0, 0, 0);

	/** Whether changing the simulation speed affects the world's time.
	* When this flag is set to true then pausing the simulation will also pause the game
	* and when the speed scale is changed then it will also change the global time dilation of the game.
	*/
	UPROPERTY(EditDefaultsOnly, Category="Simulation", meta=(EditCondition="bHasSimulation"))
	bool bIsSimulationAffectsWorldTime = false;

	/** Called when the game is initialized. */
	virtual void OnInit() {}

	/** Called when the game is started. */
	virtual void OnStart() {}

	/** Called when the game is loaded from a record. */
	virtual void OnLoadedFromRecord() {}

	/** Called when the game is ticked. */
	virtual void OnTick( float DeltaTime ) {}

	/** Called when the game simulation is ticked. */
	virtual void OnSimulationTick( const FTimespan& Timespan ) {}

	/** Called after the game has been shutdown but before it is marked as pending kill. */
	virtual void OnShutdown() {}

	/** Event triggered when the game is initialized. */
	UFUNCTION(BlueprintImplementableEvent, Category="Game", meta=(DisplayName="OnInit"))
	void BPF_OnInit();

	/** Event triggered when the game is started. */
	UFUNCTION(BlueprintImplementableEvent, Category="Game", meta=(DisplayName="OnStart"))
	void BPF_OnStart();

	/** Event triggered when the game is loaded from a record. */
	UFUNCTION(BlueprintImplementableEvent, Category="Game", meta=(DisplayName="OnLoaded"))
	void BPF_OnLoadedFromRecord();

	/** Event triggered when the game is ticked. */
	UFUNCTION(BlueprintImplementableEvent, Category="Tick", meta=(DisplayName="OnTick"))
	void BPF_OnTick( float DeltaTime );

	/** Event triggered when the game simulation is ticked. */
	UFUNCTION(BlueprintImplementableEvent, Category="Simulation", meta=(DisplayName="OnSimulationTick"))
	void BPF_OnSimulationTick( FTimespan Timespan );

private:

	/** The game object tree. */
	UPROPERTY(Instanced)
	UGameObjectTree* ObjectTree;	

	/** The current simulation date time. */
	UPROPERTY(SaveGame)	
	FDateTime SimulationDateTime;

	/** Whether the simulation is paused or not. */
	UPROPERTY(SaveGame)	
	bool bIsSimulationPaused = false;

	/** Scale of the simulation speed. */
	UPROPERTY(SaveGame)	
	float SimulationSpeedScale = 1.f;

	// Named variables

	UPROPERTY(SaveGame)
	TMap<FString, bool> GameFlags;

	UPROPERTY(SaveGame)
	TMap<FString, int32> GameInts;

	UPROPERTY(SaveGame)
	TMap<FString, float> GameFloats;

	UPROPERTY(SaveGame)
	TMap<FString, FString> GameStrings;

	// End of named variables

	/** Whether this game has been initialized and ready to be used. */
	bool bIsInitialized;

	/** Initialize this game object. */
	void Init();


	//////////////////////////////////////////////////////////////////////////
	// Named variables operation
	//////////////////////////////////////////////////////////////////////////

	template<typename T>
	FORCEINLINE T GetNamedVar( const TMap<FString, T>& Map, const FString& Name, const T& DefaultValue ) const {
		if ( Name.IsEmpty() )
		{
			return DefaultValue;
		}
		const T* p = Map.Find(Name);
		return p ? (*p) : DefaultValue;
	}

	template<typename T>
	FORCEINLINE void SetNamedVar( TMap<FString, T>& Map, const FString& Name, const T& Value )
	{
		if ( Name.IsEmpty() == false )
		{
			if ( Map.Contains( Name ) )
			{
				Map[Name] = Value;
			}
			else
			{
				Map.Add( Name, Value );
			}
		}
	}		
};