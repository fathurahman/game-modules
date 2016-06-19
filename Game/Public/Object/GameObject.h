// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "ObjectWithTags.h"
#include "GameObject.generated.h"

class UGame;

/** UGameObject. 
* A Game object that is usually a part of a Game (@see UGame)
* - Usually added to an parent container as part of an object tree, 
*   but using it as a standalone object is also common especially for unit testing.
* - Contains property that may change throughout the game Game
* - Can be saved and loaded to/from save game.
* - Can tick and have a world (thus can use WorldContextObject only functions)
* - Can be disposed.
* - Usually added as blueprint in the editor and has gameplay blueprint.
*/
UCLASS(Abstract, Blueprintable)
class GAME_API UGameObject : public UObjectWithTags
{
	GENERATED_BODY()
public:

	friend class UGameObjectContainer;
	friend class UGameObjectTree;

	/** Whether this object is allowed to do world tick. */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="GameObject")
	bool bCanTick = false;

	/** Whether this object is allowed to do simulation tick. */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="GameObject")
	bool bCanSimulationTick = false;			

	/** Get this object's ID. */
	UFUNCTION(BlueprintPure, Category="ID")
	FString GetID() const;

	//////////////////////////////////////////////////////////////////////////

	/**
	* Check whether this object is one of the descendants of another object.
	* @param	Other	The other object
	* @return true if this object is a descendant of the other object
	*/
	UFUNCTION(BlueprintPure, Category="GameObject")
	bool IsDescendantOf(UGameObjectContainer* Container) const;
	
	/** Get the parent of this object. */
	UFUNCTION(BlueprintPure, Category="GameObject")
	virtual UGameObjectContainer* GetParent() const;

	/** 
	* Set the parent of this object.
	* @param	InParent	The new parent.
	* @return false if the parent can not be changed.
	*/
	UFUNCTION(BlueprintCallable, Category="GameObject")
	virtual bool SetParent(UGameObjectContainer* Container);

	/**
	* Get the object tree where this object is attached to.
	* @return nullptr if this object is not attached to an object tree.
	*/
	UFUNCTION(BlueprintPure, Category="GameObject")
	virtual UGameObjectTree* GetObjectTree() const;

	/** Do world tick, Called by the parent of this object (but you can call it manually too)
	* @param DeltaSeconds	Seconds elapsed from the previous world tick.
	*/
	UFUNCTION(BlueprintCallable, Category="Tick")
	virtual void Tick(float DeltaTime);

	/** Do simulation tick, Called by the parent of this object (but you can call it manually too)
	* @param Timespan	Simulation time span from the previous simulation tick. 
	*/
	UFUNCTION(BlueprintCallable, Category="Simulation")
	virtual void SimulationTick(const FTimespan& Timespan);

	/** 
	* Remove this object from the object tree, do necessary clean up and mark it as pending kill.
	* Note: Weak reference to this object (TWeakObjectPtr) will not be valid after disposing.
	*		Strong reference to this object will need to check IsPendingKill()
	*/
	UFUNCTION(BlueprintCallable, Category="GameObject")
	void Dispose();

	/** Get ID or name of this object, this is useful only for debugging to identify the object. */
	FString GetIDOrName() const;

	template<class T=UGameObjectContainer>
	FORCEINLINE T* GetTypedParent() const {
		return Cast<T>( GetParent() );
	}

	// UObject interface
	virtual UWorld* GetWorld() const override;
	// End of UObject interface

protected:

	/** 
	* The ID of this object, it is guaranteed to unique from its siblings in the container. 
	* but not with other objects in other containers.
	*/
	UPROPERTY(Transient)
	FString ID;

	/** Parent of this object.  */
	UPROPERTY(Transient)
	TWeakObjectPtr<UGameObjectContainer> Parent;

protected:
	
	/** Called after this object is added to a game object tree. */
	virtual void OnAddedToObjectTree(UGameObjectTree* ToObjectTree) { ReceiveAddedToObjectTree(ToObjectTree); }

	/** Called after this object is removed from a game object tree. */
	virtual void OnRemovedFromObjectTree(UGameObjectTree* FromObjectTree) { ReceiveRemovedFromObjectTree(FromObjectTree); }

	/** Called after this object is added as a child to a parent. */
	virtual void OnAddedToParent(UGameObjectContainer* ToParent) { ReceiveAddedToParent(ToParent); }

	/** Called after this object is removed from a parent. */
	virtual void OnRemovedFromParent(UGameObjectContainer* FromParent) { ReceiveRemovedFromParent(FromParent); }

	/** Called after this object has been loaded from a save game. */
	virtual void OnLoaded() { ReceiveLoaded(); }

	/** Called before this object is marked pending kill. */
	virtual void OnDispose() { ReceiveDispose(); }

	//////////////////////////////////////////////////////////////////////////

	UFUNCTION( BlueprintImplementableEvent, Category="GameObject", meta=(DisplayName="OnAddedToObjectTree") )
	void ReceiveAddedToObjectTree(UGameObjectTree* ToObjectTree);

	UFUNCTION(BlueprintImplementableEvent, Category="GameObject", meta=(DisplayName="OnRemovedFromObjectTree"))
	void ReceiveRemovedFromObjectTree(UGameObjectTree* FromObjectTree);
	
	/** Event triggered after this object is added to a parent. */
	UFUNCTION(BlueprintImplementableEvent, Category="GameObject|Event", meta=(DisplayName="OnAddedToParent"))
	void ReceiveAddedToParent(UGameObjectContainer* ToParent);

	/** Event triggered after this object is removed from a parent. */
	UFUNCTION(BlueprintImplementableEvent, Category="GameObject|Event", meta=(DisplayName="OnRemovedFromParent"))
	void ReceiveRemovedFromParent(UGameObjectContainer* FromParent);

	/** Event triggered when this object receives World tick. */
	UFUNCTION(BlueprintImplementableEvent, Category="Tick|Event", meta=(DisplayName="OnTick"))
	void ReceiveTick(float DeltaTime);

	/** Event triggered when this object receives Simulation tick. */
	UFUNCTION(BlueprintImplementableEvent, Category="Tick|Event", meta=(DisplayName="OnSimulationTick"))
	void ReceiveSimulationTick(FTimespan Timespan);

	/** Event triggered after the Game is loaded. */
	UFUNCTION(BlueprintImplementableEvent, Category="GameObject|Event", meta=(DisplayName="OnLoaded"))
	void ReceiveLoaded();

	/** 
	 * Event triggered before this object is disposed, for a chance to do additional clean up. 
	 * Note: The game object tree connection for this object should have already been broken at this point (including access to Game)
	 *       If your intention is to add a gameplay when this object is removed then do it in OnRemovedFrom[Game/Parent]
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="GameObject|Event", meta=(DisplayName="OnDispose"))
	void ReceiveDispose();

};
