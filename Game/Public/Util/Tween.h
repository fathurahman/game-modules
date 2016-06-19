#pragma once

#include "Curves/CurveFloat.h"
#include "Easing.h"
#include "Tween.generated.h"


//////////////////////////////////////////////////////////////////////////
// Tween
//////////////////////////////////////////////////////////////////////////

/** Tween Parameters.
*/
USTRUCT(BlueprintType)
struct GAME_API FTweenParameters : public FEaseCurve
{
    GENERATED_USTRUCT_BODY()
    
	/** Tween's duration, in seconds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tween", meta=(UIMin="0"))
	float Duration;

	FTweenParameters()
		: FEaseCurve( EEaseType::Linear )
		, Duration( 0.25f )
	{
	}

	FTweenParameters( EEaseType EaseType, float InDuration = 0.5f )
		: FEaseCurve( EaseType )
		, Duration( InDuration )
	{
	}

	FTweenParameters( UCurveFloat* CurveFloat, float InDuration = 0.5f )
		: FEaseCurve( CurveFloat )
		, Duration( InDuration )
	{
	}

	FORCEINLINE float Ease( float Elapsed ) const 
	{
		return FEaseCurve::Ease( Elapsed, Duration );
	}
    
};

/** Tween
* A very simple tween
*/
UCLASS(BlueprintType)
class GAME_API UTween : public UObject
{
    GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTweening, float, Alpha);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinished);

public:

	UTween( const FObjectInitializer& ObjectInitializer );

	UFUNCTION(BlueprintCallable, Category="Tween")
	void SetParams( const FTweenParameters& InParams );

	UFUNCTION(BlueprintPure, Category="Tween")
	const FTweenParameters& GetParams() const;

	UFUNCTION(BlueprintCallable, Category="Tween")
	void Reset();

	UFUNCTION(BlueprintCallable, Category="Tween")
	void Stop();

	UFUNCTION(BlueprintPure, Category="Tween")
	bool IsTweening() const;

	UFUNCTION(BlueprintPure, Category="Tween")
	float GetAlpha() const;

	UFUNCTION(BlueprintPure, Category="Tween")
	float GetReversedAlpha() const;

	UFUNCTION(BlueprintCallable, Category="Tween")
	bool Tick( float DeltaSeconds );

	UPROPERTY(BlueprintAssignable, Category="Tween")
	FOnTweening OnTweening;

	UPROPERTY(BlueprintAssignable, Category="Tween")
	FOnFinished OnFinished;

protected:

	UPROPERTY(EditAnywhere, Category="Tween")
	FTweenParameters Params;

private:

	UPROPERTY(SaveGame)
	float Elapsed;

	UPROPERTY(SaveGame)
	float Alpha;
};

//////////////////////////////////////////////////////////////////////////
// Tweener
//////////////////////////////////////////////////////////////////////////

/** Tweener
* Tween generator.
*/
UCLASS(BlueprintType)
class GAME_API UTweener : public UObject
{
    GENERATED_BODY()
public:

	/** Create a new Tween using EaseType, you can then attach delegates to its events.
	* @param EaseType	The easing type
	* @param Duration	The duration
	* @return UTween instance
	*/
	UFUNCTION(BlueprintCallable, Category="Tween")
    UTween* Tween( EEaseType EaseType, float Duration = 0.5f );

	UFUNCTION(BlueprintCallable, Category="Tween")
	UTween* TweenWithCurveFloat( UCurveFloat* CurveFloat, float Duration = 0.5f );

	UFUNCTION(BlueprintCallable, Category="Tween")
	UTween* TweenWithParams( const FTweenParameters& Params );

	UFUNCTION(BlueprintPure, Category="Tween")
	bool IsTweening() const;

	UFUNCTION(BlueprintCallable, Category="Tween")
	void Tick( float DeltaSeconds );

private:

	UPROPERTY(Transient)
	TArray<UTween*> TweenObjects;
    
};

//////////////////////////////////////////////////////////////////////////
// Tweening Boolean
//////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct GAME_API FTweeningBooleanParameters
{
    GENERATED_USTRUCT_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tween")
    FTweenParameters TweenParamsIn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tween")
	FTweenParameters TweenParamsOut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tween")
	bool bInitialValue = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tween")
	bool bFlipFlop = false;
    
};

/** Tweening Boolean.
* Simply put, its a boolean, but it has an ''Alpha'' value following the boolean value wherever it goes.
* - Boolean value changes is instant
* - Alpha will follow boolean value with the provided tween parameters
* - Boolean value = true, Alpha = tweening to 1 using TweenParamsIn
* - Boolean value = false, Alpha = tweening to 0 using TweenParamsOut
*/
UCLASS(BlueprintType)
class GAME_API UTweeningBoolean : public UObject
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnValueChanged, bool);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnTweening, float);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnFinished, bool);	

public:

	friend class UBooleanTweener;

	FORCEINLINE static UTweeningBoolean* Create( UObject* Outer, const FTweeningBooleanParameters& Params, const FName& Name = NAME_None)
	{
		UTweeningBoolean* Object = NewObject<UTweeningBoolean>( Outer, Name );
		Object->SetParams( Params );
		return Object;
	}

	//////////////////////////////////////////////////////////////////////////

	UTweeningBoolean(const FObjectInitializer& ObjectInitializer);

	FTweeningBooleanParameters& GetParams() { return Params; }

	/** Initialize tweening boolean with a certain parameters. */
	UFUNCTION(BlueprintCallable, Category="Tween")	
	void SetParams( UPARAM(Ref) const FTweeningBooleanParameters& InParams );

	UFUNCTION(BlueprintCallable, Category="Tween")
	void ChangeDuration( float DurationIn, float DurationOut );

	/** Revert tweening boolean to its initial values. */
	UFUNCTION(BlueprintCallable, Category="Tween")	
	void Reset();

	/** Whether it's currently tweening. */
	UFUNCTION(BlueprintPure, Category="Tween")
	bool IsTweening() const;

	/** Get the current Alpha, which can further be used to interpolate any interpolatable variables. */
	UFUNCTION(BlueprintPure, Category="Tween")
	float GetAlpha() const;

	/** Get the boolean value. */
	UFUNCTION(BlueprintPure, Category="Tween")
	bool GetValue() const;

	/** Set the boolean value.
	* @param	bInValue	The new value
	* @param	bTween		Whether to tween alpha
	*/
	UFUNCTION(BlueprintCallable, Category="Tween")
	void SetValue(bool bInValue, bool bTween = true);

	/** Toggle the boolean value.
	* @param	bTween		Whether to tween alpha
	*/
	UFUNCTION(BlueprintCallable, Category="Tween")
	void ToggleValue(bool bTween = true);

	/** 
	* Tick this tweening boolean
	* @param	DeltaTime Time elapsed since the last tick.
	* @return false if it is not tweening anymore.
	*/
	UFUNCTION(BlueprintCallable, Category="Tween")
	bool Tick(float DeltaTime);

	FOnValueChanged OnValueChanged;

	FOnTweening OnTweening;

	FOnFinished OnFinished;

protected:

	UPROPERTY()
	FString ID;

	UPROPERTY(EditAnywhere, Category="Tween")
	FTweeningBooleanParameters Params;

private:

	UPROPERTY()
	bool bInitialized = false;

	UPROPERTY()
	bool bLocked = false;

	UPROPERTY(Transient)
	bool bValue = false;

	UPROPERTY(Transient)
	bool bIsTweening = false;

	UPROPERTY(Transient)
	mutable bool bInterpolateAlpha = false;

	UPROPERTY(Transient)
	float Elapsed = 0.f;

	UPROPERTY(Transient)
	mutable bool bIsDirty = false;
	
	UPROPERTY(Transient)
	mutable float Alpha = 0.f;
};

//////////////////////////////////////////////////////////////////////////
// Boolean Tweener
//////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct GAME_API FTweeningBooleanPreset
{
    GENERATED_USTRUCT_BODY()
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tween")
    FString ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tween")
	FTweeningBooleanParameters Params;
	
	//////////////////////////////////////////////////////////////////////////

	FTweeningBooleanPreset() {}

	FTweeningBooleanPreset( const FString& InID, const FTweeningBooleanParameters& InParams )
		: ID( InID )
		, Params( InParams )
	{}
    
};

/** Boolean Tweener.
* Manages TweeningBooleans
*/
UCLASS(BlueprintType)
class GAME_API UBooleanTweener : public UObject
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FOnValueChanged, FString, ID, bool, Value );
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FOnTweening, FString, ID, float, Alpha );
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FOnFinished, FString, ID, bool, Value );

public:

	UBooleanTweener( const FObjectInitializer& ObjectInitializer );

	UFUNCTION(BlueprintCallable, Category="Tween")
	void SetDefaultParams( UPARAM(Ref) const FTweeningBooleanParameters& Params );

	UFUNCTION(BlueprintCallable, Category="Tween")
	void SetParams( FString ID, UPARAM(Ref) const FTweeningBooleanParameters& Params );

	UFUNCTION(BlueprintPure, Category="Tween")
	bool GetValue( FString ID ) const;

	UFUNCTION(BlueprintPure, Category="Tween")
	float GetAlpha( FString ID ) const;

	UFUNCTION(BlueprintCallable, Category="Tween")
	void SetValue( FString ID, bool Value );

	UFUNCTION(BlueprintCallable, Category="Tween")
	void ToggleValue( FString ID );

	UFUNCTION(BlueprintPure, Category="Tween")
	bool IsTweening() const;

	UFUNCTION(BlueprintCallable, Category="Tween")
	void Tick( float DeltaTime );

	UFUNCTION(BlueprintCallable, Category="Tween")
	void Reset();

	UPROPERTY(BlueprintAssignable, Category="Tween")
	FOnValueChanged OnValuedChanged;

	UPROPERTY(BlueprintAssignable, Category="Tween")
	FOnTweening OnTweening;

	UPROPERTY(BlueprintAssignable, Category="Tween")
	FOnFinished OnFinished;

protected:

	UPROPERTY(EditAnywhere, Category="Tween")
	FTweeningBooleanParameters DefaultParams;

	UPROPERTY(EditAnywhere, Category="Tween")
	TArray<FTweeningBooleanPreset> Presets;

private:

	UFUNCTION()
	void HandleValueChanged( bool bValue, FString Name );

	UFUNCTION()
	void HandleTweening( float Alpha, FString Name );

	UFUNCTION()
	void HandleFinished( bool bValue, FString Name );

	UPROPERTY(Transient)
	TArray<UTweeningBoolean*> TweeningBooleans;

	UTweeningBoolean* GetTweeningBoolean( const FString& Name ) const;
	UTweeningBoolean* GetOrCreateTweeningBoolean( const FString& Name );

};

//////////////////////////////////////////////////////////////////////////
// Tweening State Machine
//////////////////////////////////////////////////////////////////////////

/** A Tweening state. */
USTRUCT(BlueprintType)
struct GAME_API FTweeningState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tween")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tween")
	bool bCanEnterFromAnyStates = true;

	/** A list of states it can be entered from. Only valid if bCanEnterFromAnyState == false. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tween", meta=(EditCondition="!bCanEnterFromAnyStates"))
	TArray<FString> CanEnterFrom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tween")
	FTweeningBooleanParameters TweeningParams;

	UPROPERTY()
	UTweeningBoolean* TweeningBoolean = nullptr;
};

/** Tweening State Machine.
* The way it works is similar to Simple State Machine ( @see USimpleStateMachine )
* - State changes is instant ( as long as the state change rules are satisfied )
* - Each state has a tweening boolean.
* - When the state changes, the previous state will set its tweening boolean value to false
* - When the state changes, the new state will set its tweening boolean value to true
* - Tweening boolean events are relayed using UTweeningStateMachine delegates.
*/
UCLASS(BlueprintType)
class GAME_API UTweeningStateMachine : public UObject
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStateChanged, FString, FromState, FString, ToState);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTweening, FString, CurrentStateName, float, Alpha);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFinished, FString, CurrentStateName, bool, bActive);
				

public:
	
	UTweeningStateMachine( const FObjectInitializer& ObjectInitializer );

	/** Get the current state. */
	UFUNCTION(BlueprintPure, Category="Tween")	
	const FString& GetCurrentState() const;

	/** Change the current state.
	* @param	InState		New State to change to
	* @return false if the state can not be changed.
	*/
	UFUNCTION(BlueprintCallable, Category="Tween")
	bool ChangeState( UPARAM(Ref) const FString& InState );

	/** Check whether it's currently tweening. */
	UFUNCTION(BlueprintPure, Category="Tween")	
	bool IsTweening() const;

	/** Tick this tweening state machine. */
	UFUNCTION(BlueprintCallable, Category="Tween")	
	void Tick( float DeltaTime );

	/** Event triggered when the current state changed. */
	UPROPERTY(BlueprintAssignable, Category="Tween")
	FOnStateChanged OnStateChanged;

	/** Event triggered every time it's tweening. */
	UPROPERTY(BlueprintAssignable, Category="Tween")
	FOnTweening OnTweening;

	/** Event triggered when the tween is finished.*/
	UPROPERTY(BlueprintAssignable, Category="Tween")
	FOnFinished OnFinished;

protected:

	/** Tweening state list. */
	UPROPERTY( EditDefaultsOnly, Category="Tween" )
	TArray<FTweeningState> TweeningStates;

	UPROPERTY( SaveGame, EditDefaultsOnly, Category="Tween" )
	FString CurrentStateName;

	UFUNCTION()
	void ReceiveOnTweening( float Alpha, FString State );

	UFUNCTION()
	void ReceiveOnFinished( bool bValue, FString State );

private:

	FTweeningState* GetTweeningState( const FString& State );
};