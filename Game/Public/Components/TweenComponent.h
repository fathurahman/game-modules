// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "Components/ActorComponent.h"
#include "Util/Tween.h"
#include "TweenComponent.generated.h"

UCLASS( meta=(BlueprintSpawnableComponent) )
class UTweenerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UTweenerComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category="TweenComponent")
	UTweener* GetTweener() const { return Tweener; }

	
	// UActorComponent interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	// End of UActorComponent interface

protected:

	UPROPERTY(Instanced)
	UTweener* Tweener;

};

//////////////////////////////////////////////////////////////////////////
// UTweeningBooleanComponent
//////////////////////////////////////////////////////////////////////////

UCLASS( meta=(BlueprintSpawnableComponent) )
class GAME_API UBooleanTweenerComponent : public UActorComponent
{
    GENERATED_BODY()

public:

	UBooleanTweenerComponent( const FObjectInitializer& ObjectInitializer );

	UFUNCTION(BlueprintPure, Category="TweenComponent")
	UBooleanTweener* GetBooleanTweener() const { return BooleanTweener; }

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

protected:

	UPROPERTY(VisibleAnywhere, Instanced, Category="TweenComponent")
	UBooleanTweener* BooleanTweener;
};

//////////////////////////////////////////////////////////////////////////
// UTweeningStateMachineComponent
//////////////////////////////////////////////////////////////////////////

UCLASS(meta=(BlueprintSpawnableComponent))
class GAME_API UTweeningStateMachineComponent : public UActorComponent
{
    GENERATED_BODY()
public:

	UTweeningStateMachineComponent( const FObjectInitializer& ObjectInitializer );

	UFUNCTION(BlueprintPure, Category="TweenComponent")
	UTweeningStateMachine* GetTweeningStateMachine() const { return TweeningStateMachine; }

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

protected:

	UPROPERTY(VisibleAnywhere, Instanced, Category="TweeningStateMachine")
	UTweeningStateMachine* TweeningStateMachine;
	
    
};
