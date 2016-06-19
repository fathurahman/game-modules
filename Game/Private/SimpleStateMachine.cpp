// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GamePrivatePCH.h"
#include "Util/SimpleStateMachine.h"

const FString& USimpleStateMachine::GetCurrentState() const
{
	return CurrentStateName;
}

bool USimpleStateMachine::ChangeState(const FString& StateName)
{
	if ( StateName.IsEmpty() || CurrentStateName == StateName )
	{
		// Can not change to an empty state or to the same state.
		return false;
	}

	FSimpleState* State = GetState( StateName );
	if ( !State )
	{
		Trace("Unknown state: %s", *StateName);
		return false;
	}

	if ( State->bCanEnterFromAnyStates == false && CurrentStateName.IsEmpty() == false && State->CanEnterFrom.Contains(CurrentStateName) == false )
	{
		// Condition doesn't match.
		return false;
	}

	FString PrevStateName = CurrentStateName;
	CurrentStateName = StateName;

	OnStateChanged.Broadcast( PrevStateName, CurrentStateName );

	return true;
}

FSimpleState* USimpleStateMachine::GetState(const FString& StateName)
{
	if ( StateName.IsEmpty() )
	{
		return nullptr;
	}

	for ( FSimpleState& SimpleState : SimpleStates )
	{
		if ( SimpleState.Name == StateName )
		{
			return &SimpleState;
		}
	}

	return nullptr;
}

