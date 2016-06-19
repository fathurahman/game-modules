#pragma once
#include "Object/GameObjectContainer.h"
#include "StateMachine.generated.h"

//////////////////////////////////////////////////////////////////////////
// UState
//////////////////////////////////////////////////////////////////////////

UCLASS(Abstract, Blueprintable)
class GAME_API UState : public UGameObject
{
    GENERATED_BODY()
public:

	UState( const FObjectInitializer& ObjectInitializer );

	UPROPERTY(BlueprintReadOnly, Category="StateMachine")
	TWeakObjectPtr<class UStateMachine> OwningStateMachine;

	UFUNCTION(BlueprintPure, Category="StateMachine")
	class UStateMachineComponent* GetOwningComponent() const;

	UFUNCTION(BlueprintPure, Category="StateMachine")
	AActor* GetOwningActor() const;

	bool CanEnterFrom( UState* State ) const;

	virtual void OnEnter() { ReceiveOnEnter(); }

	virtual void OnExit() { ReceiveOnExit(); }

	virtual void OnEvent( const FString& EventName, UObject* Object ) { ReceiveEvent( EventName, Object ); }

protected:

	UPROPERTY(EditDefaultsOnly, Category="StateMachine")
	bool bCanEnterFromAnyStates = true;
	
	UPROPERTY(EditAnywhere, Category="StateMachine", meta=(EditCondition="!bCanEnterFromAnyStates", MetaClass="State"))
	TArray<FStringClassReference> CanEnterFromStateClasses;

protected:

	UFUNCTION(BlueprintImplementableEvent, Category="StateMachine", meta=(DisplayName="OnEnter"))
	void ReceiveOnEnter();

	UFUNCTION(BlueprintImplementableEvent, Category="StateMachine", meta=(DisplayName="OnExit"))
	void ReceiveOnExit();

	UFUNCTION(BlueprintImplementableEvent, Category="StateMachine", meta=(DisplayName="OnEvent"))
	void ReceiveEvent( const FString& EventName, UObject* Object );
    
};


//////////////////////////////////////////////////////////////////////////
// UStateMachine
//////////////////////////////////////////////////////////////////////////

UCLASS(Blueprintable)
class GAME_API UStateMachine : public UGameObjectContainer
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStateChanged, FString, FromStateName, FString, ToStateName);
	
public:

	UStateMachine( const FObjectInitializer& ObjectInitializer );

	UPROPERTY(BlueprintReadOnly, Category="StateMachine")
	TWeakObjectPtr<class UStateMachineComponent> OwningComponent;

	/** The current state */
	UFUNCTION(BlueprintPure, Category="StateMachine")
	UState* GetCurrentStateObject() const;

	UFUNCTION(BlueprintPure, Category="StateMachine")
	UClass* GetCurrentState() const;

	/** Change to a new state, returns false if it can not change the current state. */
	UFUNCTION(BlueprintCallable, Category="StateMachine")
	bool ChangeState(TSubclassOf<UState> StateClass);

	/** Trigger an event to be passed to the current state. */
	UFUNCTION(BlueprintCallable, Category="StateMachine")
	void TriggerEvent(const FString& EventName, UObject* Object);

	UPROPERTY(BlueprintAssignable, Category="StateMachine|Event")
	FOnStateChanged OnStateChanged;

protected:

	UPROPERTY(SaveGame)
	TWeakObjectPtr<UState> CurrentState;

};