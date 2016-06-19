// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "Object/GameData.h"
#include "Object/GameObjectTree.h"
#include "Game.generated.h"


/** UGame
* A basic game Game class that stores object tree, can do simulation, and can be serialized to save game.
* @par
* Game Game is an object that should belong in a game instance and is persistent throughout the many game worlds. It should
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

	/** Get the currently running game. */
	static UGame* Get()
		;
	static FORCEINLINE UGame* GetChecked() 
	{
		UGame* Game = Get();
		check(Game);
		return Game;
	}

	static UGame* GetOrStart();

	UGame( const FObjectInitializer& ObjectInitializer );

	/**
	* Initialize Game.
	* @warning This function MUST be called with a valid GameInstance before starting or loading a Game. 
	* @param	InGameInstance		Game Instance that owns this Game.
	*/
	UFUNCTION(BlueprintCallable, Category="Game")
	void Init();

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

	/** Dispose this Game and mark it as pending kill. 
	* @note It's preferred to force garbage collection after disposing a Game because it usually does takes a lot of memory. 
	*/
	UFUNCTION(BlueprintCallable, Category="Game")
	void Dispose();

	/** Get the object tree root. */
	UFUNCTION(BlueprintPure, Category="GameObject")
	UGameObjectTree* GetObjectTree() const;	

	/** Checks whether this Game has simulation or not. */
	UFUNCTION(BlueprintPure, Category="Simulation")
	bool HasSimulation() const;

	UFUNCTION(BlueprintPure, Category="Simulation")
	bool IsSimulationPaused() const;

	UFUNCTION(BlueprintPure, Category="Simulation")
	float GetSimulationSpeedScale() const;

	UFUNCTION(BlueprintPure, Category="Simulation")
	FDateTime GetSimulationDateTime() const;

	UFUNCTION(BlueprintCallable, Category="Simulation")
	void SetIsSimulationPaused( bool bPaused );

	UFUNCTION(BlueprintCallable, Category="Simulation")
	void SetSimulationSpeedScale( float SpeedScale );

	UFUNCTION(BlueprintCallable, Category="Simulation")
	void SetSimulationDateTime( const FDateTime& InDateTime );

	UFUNCTION(BlueprintCallable, Category="Tick")
	void Tick( float DeltaTime );

	UFUNCTION(BlueprintPure, Category="Game")
	bool GetGameFlag( FString Name, bool bDefaultValue = false ) const;

	UFUNCTION(BlueprintCallable, Category="Game")
	void SetGameFlag( FString Name, bool bValue );

	UFUNCTION(BlueprintPure, Category="Game")
	int32 GetGameInt( FString Name, int32 DefaultValue = 0 ) const;

	UFUNCTION(BlueprintCallable, Category="Game")
	void SetGameInt( FString Name, int32 Value );

	UFUNCTION(BlueprintPure, Category="Game")
	float GetGameFloat( FString Name, float DefaultValue = 0.f ) const;

	UFUNCTION(BlueprintCallable, Category="Game")
	void SetGameFloat( FString Name, float Value );

	UFUNCTION(BlueprintPure, Category="Game")
	FString GetGameString( FString Name, FString DefaultValue = TEXT("") ) const;

	UFUNCTION(BlueprintCallable, Category="Game")
	void SetGameString( FString Name, FString Value );

	virtual UWorld* GetWorld() const override;

protected:

	virtual void OnInit() { ReceiveInit(); }

	virtual void OnStart() { ReceiveStart(); }

	virtual void OnLoadedFromRecord() { ReceiveLoadedFromRecord(); }

	virtual void OnDispose() { ReceiveDispose(); }

	virtual void OnTick( float DeltaTime ) { ReceiveTick( DeltaTime ); }

	virtual void OnSimulationTick( const FTimespan& Timespan ) { ReceiveSimulationTick( Timespan ); }

	UFUNCTION(BlueprintImplementableEvent, Category="Game", meta=(DisplayName="OnInit"))
	void ReceiveInit();

	UFUNCTION(BlueprintImplementableEvent, Category="Game", meta=(DisplayName="OnStart"))
	void ReceiveStart();

	UFUNCTION(BlueprintImplementableEvent, Category="Game", meta=(DisplayName="OnLoaded"))
	void ReceiveLoadedFromRecord();

	UFUNCTION(BlueprintImplementableEvent, Category="Game", meta=(DisplayName="OnDispose"))
	void ReceiveDispose();

	UFUNCTION(BlueprintImplementableEvent, Category="Tick", meta=(DisplayName="OnTick"))
	void ReceiveTick( float DeltaTime );

	UFUNCTION(BlueprintImplementableEvent, Category="Simulation", meta=(DisplayName="OnSimulationTick"))
	void ReceiveSimulationTick( FTimespan Timespan );

protected:

	UPROPERTY(EditDefaultsOnly, Category="Simulation")
	bool bHasSimulation = false;

	UPROPERTY(EditDefaultsOnly, Category="Simulation", meta=(EditCondition="bHasSimulation"))
	FDateTime InitialSimulationDateTime;

	UPROPERTY(EditDefaultsOnly, Category="Simulation", meta=(EditCondition="bHasSimulation"))
	FTimespan SimulationTimespanPerSeconds = FTimespan(0, 1, 0, 0, 0);

	UPROPERTY(EditDefaultsOnly, Category="Simulation", meta=(EditCondition="bHasSimulation"))
	bool bIsSimulationAffectsWorldTime = false;

private:


	UPROPERTY(SaveGame)	
	FDateTime SimulationDateTime;

	UPROPERTY(SaveGame)	
	bool bIsSimulationPaused = false;

	UPROPERTY(SaveGame)	
	float SimulationSpeedScale = 1.f;

	TMap<FString, bool> GameFlags;
	TMap<FString, int32> GameInts;
	TMap<FString, float> GameFloats;
	TMap<FString, FString> GameStrings;

	UPROPERTY(Instanced)
	UGameObjectTree* ObjectTree;	

	//////////////////////////////////////////////////////////////////////////
	// Game Map operations
	//////////////////////////////////////////////////////////////////////////

	template<typename T>
	FORCEINLINE T GetGameMapValue( const TMap<FString, T>& Map, const FString& Name, const T& DefaultValue ) const {
		if ( Name.IsEmpty() )
		{
			return DefaultValue;
		}
		const T* p = Map.Find(Name);
		return p ? (*p) : DefaultValue;
	}

	template<typename T>
	FORCEINLINE void SetGameMapValue( TMap<FString, T>& Map, const FString& Name, const T& Value )
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