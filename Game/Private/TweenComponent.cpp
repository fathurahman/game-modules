// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GamePrivatePCH.h"
#include "Components/TweenComponent.h"

UTweenerComponent::UTweenerComponent(const FObjectInitializer& ObjectInitializer)
	: Super( ObjectInitializer )
{
	Tweener = ObjectInitializer.CreateDefaultSubobject<UTweener>(this, "Tweener_Default");

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.SetTickFunctionEnable( true );
}

void UTweenerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	Tweener->Tick( DeltaTime );
}

//////////////////////////////////////////////////////////////////////////
// UTweeningBooleanComponent
//////////////////////////////////////////////////////////////////////////

UBooleanTweenerComponent::UBooleanTweenerComponent(const FObjectInitializer& ObjectInitializer)
	: Super( ObjectInitializer )
{
	BooleanTweener = ObjectInitializer.CreateDefaultSubobject<UBooleanTweener>(this, "BooleanTweenerDefault");
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.SetTickFunctionEnable( true );
}


void UBooleanTweenerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
	BooleanTweener->Tick( DeltaTime );
}

//////////////////////////////////////////////////////////////////////////
// UTweeningStateMachineComponent
//////////////////////////////////////////////////////////////////////////

UTweeningStateMachineComponent::UTweeningStateMachineComponent(const FObjectInitializer& ObjectInitializer)
	: Super( ObjectInitializer )
{
	TweeningStateMachine = ObjectInitializer.CreateDefaultSubobject<UTweeningStateMachine>(this, "TweeningStateMachineDefault");

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.SetTickFunctionEnable( true );
}

void UTweeningStateMachineComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	TweeningStateMachine->Tick( DeltaTime );
}
