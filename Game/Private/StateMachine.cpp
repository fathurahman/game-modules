// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GamePrivatePCH.h"
#include "Components/StateMachineComponent.h"

UState::UState(const FObjectInitializer& ObjectInitializer)
	: Super( ObjectInitializer )
{
}

UStateMachineComponent* UState::GetOwningComponent() const
{
	return OwningStateMachine.IsValid() ? OwningStateMachine->OwningComponent.Get() : nullptr;
}

AActor* UState::GetOwningActor() const
{
	UStateMachineComponent* Comp = GetOwningComponent();
	return Comp ? Comp->GetOwner() : nullptr;
}

bool UState::CanEnterFrom(UState* State) const
{
	if ( bCanEnterFromAnyStates )
	{
		return true;
	}

	UClass* StateClass = State->GetClass();

	for ( const FStringClassReference& ClassRef : CanEnterFromStateClasses )
	{
		if ( ClassRef.IsValid() )
		{
			if ( StateClass == ClassRef.ResolveClass() )
			{
				return true;
			}
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
// UStateMachine
//////////////////////////////////////////////////////////////////////////

UStateMachine::UStateMachine(const FObjectInitializer& ObjectInitializer)
	: Super( ObjectInitializer )
{
	bCanTick = true;
	bCanSimulationTick = true;
	bAllowChildrenToTick = true;
}


UState* UStateMachine::GetCurrentStateObject() const
{
	return CurrentState.Get();
}

UClass* UStateMachine::GetCurrentState() const
{
	return CurrentState.IsValid() ? CurrentState->GetClass() : nullptr;
}

bool UStateMachine::ChangeState(TSubclassOf<UState> StateClass)
{
	UClass* CurrentStateClass = CurrentState.IsValid() ? CurrentState->GetClass() : nullptr;

	if ( !StateClass || StateClass == CurrentStateClass )
	{
		return false;
	}

	UState* StateCDO = StateClass->GetDefaultObject<UState>();
	if ( StateCDO->CanEnterFrom( CurrentState.Get() ) == false )
	{
		return false;
	}

	FString PrevStateName = CurrentStateClass ? CurrentStateClass->GetName() : TEXT("");
	UState* PrevState = CurrentState.Get();

	FString StateName = StateClass->GetName();
	UState* State = NewObject<UState>( this );
	State->OwningStateMachine = this;

	CurrentState = State;

	if ( PrevState )
	{
		PrevState->OnExit();
		RemoveChild( PrevState, true );
	}

	AddChild( State );
	State->OnEnter();

	OnStateChanged.Broadcast( PrevStateName, StateName );

	return true;
}

void UStateMachine::TriggerEvent(const FString& EventName, UObject* Object)
{
	UState* State = CurrentState.Get();
	if ( State )
	{
		State->OnEvent( EventName, Object );
	}
}
