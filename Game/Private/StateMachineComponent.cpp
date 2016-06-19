// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GamePrivatePCH.h"
#include "Components/StateMachineComponent.h"

UStateMachineComponent::UStateMachineComponent(const FObjectInitializer& ObjectInitializer)
	: Super( ObjectInitializer )
{
	StateMachine = ObjectInitializer.CreateDefaultSubobject<UStateMachine>(this, "StateMachineDefault");
	StateMachine->OwningComponent = this;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.SetTickFunctionEnable(true);
}

void UStateMachineComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	StateMachine->Tick( DeltaTime );
}

USimpleStateMachineComponent::USimpleStateMachineComponent(const FObjectInitializer& ObjectInitializer)
	: Super( ObjectInitializer )
{
	SimpleStateMachine = ObjectInitializer.CreateDefaultSubobject<USimpleStateMachine>(this, "SimpleStateMachineDefault");
}