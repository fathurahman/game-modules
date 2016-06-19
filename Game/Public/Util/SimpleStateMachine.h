#pragma once
#include "SimpleStateMachine.generated.h"

/** Simple State. With entering rules and name. */
USTRUCT(BlueprintType)
struct GAME_API FSimpleState
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SimpleStateInfo")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SimpleStateMachine")
	bool bCanEnterFromAnyStates = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SimpleStateInfo", meta=(EditCondition="!bCanEnterFromAnyStates"))
	TArray<FString> CanEnterFrom;
};

/**
* A simple state machine for any purpose.
*/
UCLASS(Blueprintable, BlueprintType)
class GAME_API USimpleStateMachine: public UObject
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStateChanged, FString, FromState, FString, ToState);

public:

	/** Get the current state. */
	UFUNCTION(BlueprintPure, Category="SimpleState")
	const FString& GetCurrentState() const;

	/**
	* Change the current state.
	* @param	State	State to change to.
	* @return false if the current state can not be changed.
	*/
	UFUNCTION(BlueprintCallable,Category="SimpleState")
	bool ChangeState(UPARAM(Ref) const FString& State);

	/** Triggered whenever the current state changed. */
	UPROPERTY(BlueprintAssignable ,Category="SimpleState|Event")
	FOnStateChanged OnStateChanged;

protected:

	UPROPERTY(SaveGame, EditDefaultsOnly, Category="SimpleState")
	FString CurrentStateName;

	UPROPERTY(EditAnywhere, Category="SimpleState")
	TArray<FSimpleState> SimpleStates;

	FSimpleState* GetState( const FString& StateName );
};

