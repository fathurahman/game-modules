// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GamePrivatePCH.h"
#include "Util/Tween.h"

//////////////////////////////////////////////////////////////////////////
// UTween
//////////////////////////////////////////////////////////////////////////

UTween::UTween(const FObjectInitializer& ObjectInitializer)
	: Super( ObjectInitializer )
{
	Elapsed = -1;
	Params.EaseType = EEaseType::Linear;
	Params.Duration = 0.5f;
}

void UTween::SetParams(const FTweenParameters& InParams)
{
	Params = InParams;
	Reset();
}

const FTweenParameters& UTween::GetParams() const
{
	return Params;
}

void UTween::Reset()
{
	Elapsed = 0;
	Alpha = 0.f;
}


void UTween::Stop()
{
	Elapsed = Params.Duration;
}

bool UTween::IsTweening() const
{
	return Elapsed >= 0 && Elapsed < Params.Duration;
}

float UTween::GetAlpha() const
{
	return Alpha;
}

float UTween::GetReversedAlpha() const
{
	return 1.f - Alpha;
}

bool UTween::Tick(float DeltaSeconds)
{
	if ( Elapsed < 0 || Elapsed >= Params.Duration )
	{
		return false;
	}

	Elapsed += DeltaSeconds;
	if ( Elapsed >= Params.Duration )
	{
		Alpha = 1.f;
		OnTweening.Broadcast( Alpha );
		OnFinished.Broadcast();
		return false;
	}

	Alpha = Params.Ease( Elapsed );
	OnTweening.Broadcast( Alpha );
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Tweening Boolean
//////////////////////////////////////////////////////////////////////////

UTweeningBoolean::UTweeningBoolean(const FObjectInitializer& ObjectInitializer)
	: Super( ObjectInitializer )
{
}

void UTweeningBoolean::SetParams(const FTweeningBooleanParameters& InParams)
{
	Params = InParams;
	if ( bInitialized )
	{
		Reset();
	}
}


void UTweeningBoolean::ChangeDuration( float DurationIn, float DurationOut )
{
	Params.TweenParamsIn.Duration = DurationIn;
	Params.TweenParamsOut.Duration = DurationOut;
}

void UTweeningBoolean::Reset()
{
	if ( bLocked ) return;
	bLocked = true;

	bValue = Params.bInitialValue;
	Alpha = bValue ? 1.f : 0.f;
	bIsTweening = false;
	Elapsed = 0.f;

	bInitialized = true;

	bLocked = false;

	if ( Params.bFlipFlop )
	{
		ToggleValue();
	}
}

bool UTweeningBoolean::IsTweening() const
{
	return bIsTweening;
}

float UTweeningBoolean::GetAlpha() const
{
	if ( !bInitialized )
	{
		return Params.bInitialValue ? 1.0f : 0.f;
	}

	if ( bIsDirty )
	{
		if ( bIsTweening == false )
		{
			Alpha = bValue ? 1.0f : 0.f;
		}
		else
		{
			const FTweenParameters& TweenParams = bValue ? Params.TweenParamsIn : Params.TweenParamsOut;
			float TargetAlpha = TweenParams.Ease( Elapsed );
			if ( !bInterpolateAlpha )
			{
				Alpha = TargetAlpha;
			}
			else
			{
				Alpha = FMath::Lerp( Alpha, TargetAlpha, 0.75f );
				if ( FMath::IsNearlyEqual(Alpha, TargetAlpha) )
				{
					bInterpolateAlpha = false;
				}
			}
			if ( !bValue )
			{
				Alpha = 1.f - Alpha;
			}
		}

		bIsDirty = false;
	}

	return Alpha;
}

bool UTweeningBoolean::GetValue() const
{
	if ( !bInitialized )
	{
		return Params.bInitialValue;
	}
	return bValue;
}

void UTweeningBoolean::SetValue(bool bInValue, bool bTween /*= false*/)
{
	if ( bInitialized == false )
	{
		Reset();
	}

	if ( bLocked ) return;
	bLocked = true;

	bool bChanged = false;
	if ( bValue != bInValue )
	{
		bChanged = true;
		bValue = bInValue;

		const FTweenParameters& CurTweenParams = bValue ? Params.TweenParamsIn : Params.TweenParamsOut;

		if ( !bTween || CurTweenParams.Duration <= 0 )
		{
			bIsTweening = false;
		}
		else if ( !bIsTweening )
		{
			// New Tween
			bIsTweening = true;
			bInterpolateAlpha = false;
			Elapsed = 0;
		}
		else
		{
			// Reversing tween
			const FTweenParameters& PrevTweenParams = bValue ? Params.TweenParamsOut : Params.TweenParamsIn;
			bInterpolateAlpha = true;
			float r = 1.0f - (Elapsed / PrevTweenParams.Duration);
			Elapsed = r * CurTweenParams.Duration;
		}

		bIsDirty = true;

		OnValueChanged.Broadcast( bValue );
		OnTweening.Broadcast( GetAlpha() );
	}

	bLocked = false;

	if ( bChanged && !bIsTweening )
	{
		OnFinished.Broadcast( bValue );
	}
}

void UTweeningBoolean::ToggleValue(bool bTween /*= false*/)
{
	SetValue( !bValue, bTween );
}

bool UTweeningBoolean::Tick(float DeltaSeconds)
{
	if ( !bIsTweening )
	{
		return false;
	}

	Elapsed += DeltaSeconds;

	const FTweenParameters& TweenParams = bValue ? Params.TweenParamsIn : Params.TweenParamsOut;

	if ( Elapsed >= TweenParams.Duration )
	{
		bIsTweening = false;
	}

	bIsDirty = true;

	float Alpha = GetAlpha();

	OnTweening.Broadcast( Alpha );

	if ( !bIsTweening )
	{
		if ( Params.bFlipFlop )
		{
			ToggleValue();
		}
		else
		{
			OnFinished.Broadcast( bValue );
		}
	}

	return bIsTweening;
}

//////////////////////////////////////////////////////////////////////////
// Tweening State Machine
//////////////////////////////////////////////////////////////////////////

UTweeningStateMachine::UTweeningStateMachine(const FObjectInitializer& ObjectInitializer)
	: Super( ObjectInitializer )
{

}

const FString& UTweeningStateMachine::GetCurrentState() const
{
	return CurrentStateName;
}

bool UTweeningStateMachine::ChangeState(const FString& InState)
{
	if ( InState.IsEmpty() || InState == CurrentStateName )
	{
		// InState is not valid or it's the same as the current state.
		return false;
	}

	FTweeningState* NextTweeningState = GetTweeningState( InState );
	if ( !NextTweeningState )
	{
		PrintLogError( "Unknown state : %s", *InState );
		return false;
	}

	FTweeningState* PrevTweeningState = GetTweeningState( CurrentStateName );
	if ( PrevTweeningState )
	{
		if ( !PrevTweeningState->bCanEnterFromAnyStates && PrevTweeningState->CanEnterFrom.Contains( InState ) == false )
		{
			// Can not enter the new state because it's not allowed to.
			return false;
		}
	}

	FString PrevState = CurrentStateName;
	CurrentStateName = InState;

	if ( PrevTweeningState )
	{
		PrevTweeningState->TweeningBoolean->SetValue( false );
	}

	if ( NextTweeningState->TweeningBoolean == nullptr )
	{
		NextTweeningState->TweeningParams.bInitialValue = false;
		NextTweeningState->TweeningBoolean = UTweeningBoolean::Create( (UObject*) this, NextTweeningState->TweeningParams );
		NextTweeningState->TweeningBoolean->OnTweening.AddUObject( this, &UTweeningStateMachine::ReceiveOnTweening, CurrentStateName );
		NextTweeningState->TweeningBoolean->OnFinished.AddUObject( this, &UTweeningStateMachine::ReceiveOnFinished, CurrentStateName );
	}

	NextTweeningState->TweeningBoolean->SetValue( true );	

	OnStateChanged.Broadcast( PrevState, CurrentStateName );

	return true;
}

bool UTweeningStateMachine::IsTweening() const
{
	for ( const FTweeningState& State : TweeningStates )
	{
		if ( State.TweeningBoolean && State.TweeningBoolean->IsTweening() )
		{
			return true;
		}
	}
	return false;
}

void UTweeningStateMachine::Tick(float DeltaSeconds)
{
	for ( FTweeningState& State : TweeningStates )
	{
		if ( State.TweeningBoolean && State.TweeningBoolean->IsTweening() )
		{
			State.TweeningBoolean->Tick( DeltaSeconds );
		}
	}
}

void UTweeningStateMachine::ReceiveOnTweening(float Alpha, FString State)
{
	OnTweening.Broadcast( State, Alpha );
}

void UTweeningStateMachine::ReceiveOnFinished(bool bValue, FString State)
{
	OnFinished.Broadcast( State, bValue );
}

FTweeningState* UTweeningStateMachine::GetTweeningState(const FString& State)
{
	if ( State.IsEmpty() )
	{
		return nullptr;
	}

	for ( FTweeningState& TweeningState : TweeningStates )
	{
		if ( TweeningState.Name == State )
		{
			return &TweeningState;
		}
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////
// UTweener
//////////////////////////////////////////////////////////////////////////

UTween* UTweener::Tween(EEaseType EaseType, float Duration /*= 0.5f */)
{
	UTween* TweenObject = NewObject<UTween>( (UObject*) this );
	TweenObject->SetParams( FTweenParameters( EaseType, Duration ) );
	TweenObjects.AddUnique( TweenObject );
	return TweenObject;
}

UTween* UTweener::TweenWithCurveFloat(UCurveFloat* CurveFloat, float Duration /*= 0.5f */)
{
	UTween* TweenObject = NewObject<UTween>( (UObject*) this );
	TweenObject->SetParams( FTweenParameters( CurveFloat, Duration ) );
	TweenObjects.AddUnique( TweenObject );
	return TweenObject;
}

UTween* UTweener::TweenWithParams(const FTweenParameters& Params)
{
	UTween* TweenObject = NewObject<UTween>( (UObject*) this );
	TweenObject->SetParams( Params );
	TweenObjects.AddUnique( TweenObject );
	return TweenObject;
}

bool UTweener::IsTweening() const
{
	return TweenObjects.Num() > 0;
}

void UTweener::Tick(float DeltaSeconds)
{
	TArray<UTween*> TweenObjectsCopy = TweenObjects;
	TweenObjects.Empty();

	for ( UTween* TweenObject : TweenObjectsCopy )
	{
		if ( TweenObject->Tick( DeltaSeconds ) == false )
		{
			TweenObject->MarkPendingKill();
		}
		else
		{
			TweenObjects.AddUnique( TweenObject );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// UBooleanTweener
//////////////////////////////////////////////////////////////////////////

UBooleanTweener::UBooleanTweener(const FObjectInitializer& ObjectInitializer)
	: Super( ObjectInitializer )
{

}

void UBooleanTweener::SetDefaultParams(const FTweeningBooleanParameters& Params)
{
	DefaultParams = Params;
}

void UBooleanTweener::SetParams(FString ID, const FTweeningBooleanParameters& Params)
{
	// If there's already an tween instance, modify it.
	UTweeningBoolean* BooleanTween = GetTweeningBoolean( ID );
	if ( BooleanTween )
	{
		BooleanTween->SetParams( Params );
		return;
	}

	// If not, then modify or add info.
	for ( FTweeningBooleanPreset& Preset : Presets )
	{
		if ( Preset.ID == ID )
		{
			Preset.Params = Params;
			return;
		}
	}
	Presets.Add( FTweeningBooleanPreset(ID, Params) );
}


bool UBooleanTweener::GetValue(FString ID) const
{
	if (UTweeningBoolean* TweeningBoolean = GetTweeningBoolean( ID ))
	{
		return TweeningBoolean->GetValue();
	}
	for ( const FTweeningBooleanPreset& Preset : Presets )
	{
		if ( Preset.ID == ID )
		{
			return Preset.Params.bInitialValue;
		}
	}
	return false;
}


float UBooleanTweener::GetAlpha(FString ID) const
{
	if ( UTweeningBoolean* TweeningBoolean = GetTweeningBoolean( ID ) )
	{
		return TweeningBoolean->GetAlpha();
	}
	for ( const FTweeningBooleanPreset& Info : Presets )
	{
		if ( Info.ID == ID )
		{
			return Info.Params.bInitialValue ? 1.f : 0.f;
		}
	}
	return 0.f;
}


void UBooleanTweener::SetValue(FString ID, bool Value)
{
	UTweeningBoolean* TweeningBoolean = GetOrCreateTweeningBoolean( ID );
	TweeningBoolean->SetValue( Value );
}


void UBooleanTweener::ToggleValue(FString ID)
{
	SetValue( ID, !GetValue( ID ) );
}


bool UBooleanTweener::IsTweening() const
{
	for ( UTweeningBoolean* i : TweeningBooleans )
	{
		if ( i->IsTweening() )
		{
			return true;
		}
	}
	return false;
}


void UBooleanTweener::Tick(float DeltaTime)
{
	for ( UTweeningBoolean* i : TweeningBooleans )
	{
		if ( i->IsTweening() )
		{
			i->Tick( DeltaTime );
		}
	}
}


void UBooleanTweener::Reset()
{
	for ( UTweeningBoolean* i : TweeningBooleans )
	{
		i->Reset();
	}
}

void UBooleanTweener::HandleValueChanged(bool bValue, FString Name)
{
	OnValuedChanged.Broadcast( Name, bValue );
}


void UBooleanTweener::HandleTweening(float Alpha, FString Name)
{
	UTweeningBoolean* TweeningBoolean = GetTweeningBoolean( Name );
	OnTweening.Broadcast( Name, Alpha );
}


void UBooleanTweener::HandleFinished(bool bValue, FString Name)
{
	OnFinished.Broadcast( Name, bValue );
}

UTweeningBoolean* UBooleanTweener::GetTweeningBoolean(const FString& ID) const
{
	for ( UTweeningBoolean* TweeningBoolean : TweeningBooleans )
	{
		if ( TweeningBoolean->ID == ID )
		{
			return TweeningBoolean;
		}
	}
	return nullptr;
}

UTweeningBoolean* UBooleanTweener::GetOrCreateTweeningBoolean(const FString& ID)
{
	UTweeningBoolean* TweeningBoolean = GetTweeningBoolean( ID );
	if ( !TweeningBoolean )
	{
		FTweeningBooleanParameters Params = DefaultParams;
		for ( const FTweeningBooleanPreset& Info : Presets )
		{
			if ( Info.ID == ID )
			{
				Params = Info.Params;
			}
		}

		TweeningBoolean = NewObject<UTweeningBoolean>( this );
		TweeningBoolean->SetParams( Params );
		TweeningBoolean->ID = ID;

		TweeningBoolean->OnValueChanged.AddUObject( this, &UBooleanTweener::HandleValueChanged, ID );
		TweeningBoolean->OnTweening.AddUObject(		this, &UBooleanTweener::HandleTweening,		ID );
		TweeningBoolean->OnFinished.AddUObject(		this, &UBooleanTweener::HandleFinished,		ID );

		TweeningBooleans.Add( TweeningBoolean );
	}
	
	return TweeningBoolean;
}
