// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once
#include "Util/StateMachine.h"
#include "Util/SimpleStateMachine.h"
#include "StateMachineComponent.generated.h"

UCLASS(meta=(BlueprintSpawnableComponent))
class GAME_API UStateMachineComponent : public UActorComponent
{
    GENERATED_BODY()
public:

	UStateMachineComponent( const FObjectInitializer& ObjectInitializer );

	UFUNCTION(BlueprintPure, Category="StateMachine")
	UStateMachine* GetStateMachine() const { return StateMachine; }

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

protected:

	UPROPERTY(VisibleAnywhere, Instanced, Category="StateMachine")
	UStateMachine* StateMachine;
    
};

UCLASS(meta=(BlueprintSpawnableComponent))
class GAME_API USimpleStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()
public:

	USimpleStateMachineComponent( const FObjectInitializer& ObjectInitializer );

	UFUNCTION(BlueprintPure, Category="StateMachineComponent")
	USimpleStateMachine* GetSimpleStateMachine() const { return SimpleStateMachine; }


protected:
	
	UPROPERTY(VisibleAnywhere, Instanced, Category="StateMachine")
	USimpleStateMachine* SimpleStateMachine;
	
};