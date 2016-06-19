// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "GameTypes.h"
#include "CoreUtil.h"
#include "GameObject.h"
#include "GameObjectContainer.generated.h"

/**
* Game Object Container.
* As the name implies, this object that can contain another game object(s) as its children.
*/
UCLASS(Blueprintable)
class GAME_API UGameObjectContainer : public UGameObject
{
    GENERATED_BODY()
public:

	friend class UGameObjectTree;

	//////////////////////////////////////////////////////////////////////////
	// Unreal Properties and Functions
	//////////////////////////////////////////////////////////////////////////

	/** Whether to allow children to tick (either World or Simulation tick). */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="GameObjectContainer")
	bool bAllowChildrenToTick = false;	
    
	/**
	* Check whether this container is one of the ancestor of another object.
	* @param	Other	The other object
	* @return true if this object is an ancestor of the other object.
	*/
	UFUNCTION(BlueprintPure, Category="GameObjectContainer")
	bool IsAncestorOf( UGameObject* Other ) const;
    
	/**
	* Add an object as a child of this container
	* @param	InChild		The child to be added.
	* @param	InID		The child's ID, if this is not specified then it will use a generated ID.
	* @param	bOverwrite	If this flag is set to true and if there's an existing child with the same ID in this container, 
	*						then the existing child will be overwritten.
	* @return the added child.
	*/
	UFUNCTION(BlueprintCallable, Category="GameObjectContainer", meta=(DisplayName="AddChild", ExpandEnumAsExecs="OutResult"))
	UGameObject* BPF_AddChild( UGameObject* InChild, const FString& InID, bool bOverwrite, ESuccess& OutResult );

	/**
	* Create a child for this container.
	* This operation may fail if there's an ID clash with an existing child in this container.
	* @param	InClass		The child's class.
	* @param	InID		ID of the child, if it's not specified then it will use a generated ID.
	* @return the created child.
	*/
	UFUNCTION(BlueprintCallable, Category="GameObjectContainer", meta=(DisplayName="CreateChild", ExpandEnumAsExecs="OutResult"))
	UGameObject* BPF_CreateChild( TSubclassOf<UGameObject> InClass, const FString& InID, ESuccess& OutResult );

	/**
	* Get child with a specific ID, if there is no such child then a new one will be created.
	* @param	InClass		The child's class.
	* @param	InID		The child's ID.
	* @return the child.
	*/
	UFUNCTION(BlueprintCallable, Category="GameObjectContainer", meta=(DisplayName="GetOrCreateChild"))
	UGameObject* BPF_GetOrCreateChild( TSubclassOf<UGameObject> InClass, const FString& InID );

	/**
	* Removes a child from this container
	* @param	InChild		Child to be removed
	* @param	bDispose	Whether to dispose the removed child.
	*/
	UFUNCTION(BlueprintCallable, Category="GameObjectContainer")
	void RemoveChild( UGameObject* InChild, bool bDispose = true );

	/**
	* Removes all children from this container.
	* @param	bDispose	Whether to dispose the removed children.
	*/
	UFUNCTION(BlueprintCallable, Category="GameObjectContainer")
	void RemoveChildren( bool bDispose = true );

	/**
	* Get all children of this container.
	*/
	UFUNCTION(BlueprintPure, Category="GameObjectContainer", meta=(DisplayName="GetChildren"))
	void GetChildren( TArray<UGameObject*>& OutChildren ) const;

	/**
	* Find a child with some specific criteria.
	* @param 	InClass			The child's class, if this is not specified then any class will do.
	* @param	InID 			The child's ID, if this is not specified then any ID will do.
	* @param	InTag			The child's Tag, if this is not specified then any tag will do.
	* @return the first child that meets the criteria or null if such child does not exist.
	*/
	UFUNCTION(BlueprintPure, Category="GameObjectContainer", meta=(DisplayName="FindChild"))
	UGameObject* BPF_FindChild( TSubclassOf<UGameObject> InClass, const FString& InID, const FString& InTag) const;

	/**
	* Find a child with some specific criteria.
	* @param 	InClass			The child's class, if this is not specified then any class will do.
	* @param	InID 			The child's ID, if this is not specified then any ID will do.
	* @param	InTag			The child's Tag, if this is not specified then any tag will do.
	* @return the first child that meets the criteria or null if such child does not exist.
	*/
	UFUNCTION(BlueprintCallable, Category="GameObjectContainer", meta=(DisplayName="FindChild?", ExpandEnumAsExecs="OutResult"))
	UGameObject* BPF_FindChildEx( TSubclassOf<UGameObject> InClass, const FString& InID, const FString& InTag, EFindResult& OutResult ) const;

	/**	
	* Find children with some specific criteria.
	* @param	InClass 		The child's class, if this is not specified then any class will do.
	* @param	InTag			The child's Tag, if this is not specified then any Tag will do.
	*/
	UFUNCTION(BlueprintPure, Category="GameObjectContainer", meta=(DisplayName="FindChildren"))
	void BPF_FindChildren( TSubclassOf<UGameObject> InClass, const FString& InTag, TArray<UGameObject*>& OutChildren) const;

	/**	
	* Find children with some specific criteria.
	* @param	InClass 		The child's class, if this is not specified then any class will do.
	* @param	InTag			The child's Tag, if this is not specified then any Tag will do.
	*/
	UFUNCTION(BlueprintCallable, Category="GameObjectContainer", meta=(DisplayName="FindChildren?", ExpandEnumAsExecs="OutResult"))
	void BPF_FindChildrenEx( TSubclassOf<UGameObject> InClass, const FString& InTag, TArray<UGameObject*>& OutChildren, EFindResult& OutResult) const;

	/**
	* Get all descendants of this container.
	*/
	UFUNCTION(BlueprintPure, Category="GameObjectContainer", meta=(DisplayName="GetDescendants"))
	void GetDescendants( TArray<UGameObject*>& OutDescendants ) const;

	/**
	* Find a descendant with some specific criteria.
	* @param 	InClass			The descendant's class, if this is not specified then any class will do.
	* @param	InID 			The descendant's ID, if this is not specified then any ID will do.
	* @param	InTag			The descendant's Tag, if this is not specified then any tag will do.
	* @return the first descendant that meets the criteria or null if such descendant does not exist.
	*/
	UFUNCTION(BlueprintPure, Category="GameObjectContainer", meta=(DisplayName="FindDescendant"))
	UGameObject* BPF_FindDescendant( TSubclassOf<UGameObject> InClass, const FString& InID, const FString& InTag) const;

	/**
	* Find a descendant with some specific criteria.
	* @param 	InClass			The descendant's class, if this is not specified then any class will do.
	* @param	InID 			The descendant's ID, if this is not specified then any ID will do.
	* @param	InTag			The descendant's Tag, if this is not specified then any tag will do.
	* @return the first descendant that meets the criteria or null if such descendant does not exist.
	*/
	UFUNCTION(BlueprintCallable, Category="GameObjectContainer", meta=(DisplayName="FindDescendant?", ExpandEnumAsExecs="OutResult"))
	UGameObject* BPF_FindDescendantEx( TSubclassOf<UGameObject> InClass, const FString& InID, const FString& InTag, EFindResult& OutResult ) const;

	/**
	* Find descendants with some specific criteria.
	* @param	InClass			The descendant's class, if this is not specified then any class will do.
	* @param	InTag			The descendant's tag, if this is not specified then any Tag will do.
	*/
	UFUNCTION(BlueprintPure, Category="GameObjectContainer", meta=(DisplayName="FindDescendants"))
	void BPF_FindDescendants( TSubclassOf<UGameObject> InClass, const FString& InTag, TArray<UGameObject*>& OutDescendants ) const;

	/**
	* Find descendants with some specific criteria.
	* @param	InClass			The descendant's class, if this is not specified then any class will do.
	* @param	InTag			The descendant's tag, if this is not specified then any Tag will do.
	*/
	UFUNCTION(BlueprintCallable, Category="GameObjectContainer", meta=(DisplayName="FindDescendants?", ExpandEnumAsExecs="OutResult"))
	void BPF_FindDescendantsEx( TSubclassOf<UGameObject> InClass, const FString& InTag, TArray<UGameObject*>& OutDescendants, EFindResult& OutResult ) const;

	//////////////////////////////////////////////////////////////////////////
	// Native Properties and Functions
	//////////////////////////////////////////////////////////////////////////

	/** Constructor. */
    UGameObjectContainer(const FObjectInitializer& ObjectInitializer);

	/**
	* Find a specific child in this container that meets some certain criteria.
	* @param	InClass		The child's class, if this is not specified then any class will do.
	* @param	InID		The child's ID, if this is not specified then any ID will do.
	* @param	InTag		The child's Tag, if this is not specified then any Tag will do.
	* @return the first child that meets the criteria.
	*/
	template<class T=UGameObject>
	FORCEINLINE T* FindChild( TSubclassOf<T> InClass, const FString& InID = TEXT(""), const FString& InTag = TEXT("") ) const 
	{
		return Cast<T>( FindChild_Internal( InClass, InID, InTag, false  ) );
	}

	/**
	* Find a specific child in this container that meets some certain criteria.
	* @param	InID		The child's ID, if this is not specified then any ID will do.
	* @param	InTag		The child's Tag, if this is not specified then any Tag will do.
	* @return the first child that meets the criteria.
	*/
	template<class T=UGameObject>
	FORCEINLINE T* FindChild( const FString& InID = TEXT(""), const FString& InTag = TEXT("") ) const
	{
		return Cast<T>( FindChild_Internal( T::StaticClass(), InID, InTag, false ) );
	}

	/**
	* Find a specific descendant in this container that meets some certain criteria.
	* @param	InClass		The descendant's class.
	* @param	InID		The descendant's ID, if this is not specified then any ID will do.
	* @param	InTag		The descendant's Tag, if this is not specified then any Tag will do.
	* @return the first descendant that meets the criteria.
	*/
	template<class T=UGameObject>
	FORCEINLINE T* FindDescendant( TSubclassOf<T> InClass, const FString& InID = TEXT(""), const FString& InTag = TEXT("") ) const
	{
		return Cast<T>( FindChild_Internal( *InClass ? InClass : T::StaticClass(), InID, InTag, true ) );
	}

	/**
	* Find a specific descendant in this container that meets some certain criteria.
	* @param	InID		The descendant's ID, if this is not specified then any ID will do.
	* @param	InTag		The descendant's Tag, if this is not specified then any Tag will do.
	* @return the first descendant that meets the criteria.
	*/
	template<class T=UGameObject>
	FORCEINLINE T* FindDescendant( const FString& InID = TEXT(""), const FString& InTag = TEXT("") ) const
	{
		return Cast<T>( FindChild_Internal( T::StaticClass(), InID, InTag, true ) );
	}

	/**
	* Find children that meets some certain criteria.
	* @param[out]	OutChildren		The find result
	* @param[in]	InClass			The children's class.
	* @param[in]	InTag			The children's tag, if this is not specified then any Tag will do.
	* @return the number of children that match the criteria.
	*/
	template<class T=UGameObject>
	FORCEINLINE int32 FindChildren( TArray<T*>& OutChildren, TSubclassOf<T> InClass, const FString& InTag = TEXT("") ) const 
	{
		TArray<UGameObject*> Objects;
		int32 Num = FindChildren_Internal( *InClass ? InClass : T::StaticClass(), InTag, Objects, false );
		ReinterpretObjectArray( Objects, OutChildren );
		return Num;
	}

	/**
	* Find children that meets some certain criteria.
	* @param[out]	OutChildren		The find result
	* @param[in]	InTag			The children's tag, if this is not specified then any Tag will do.
	* @return the number of children that match the criteria.
	*/
	template<class T=UGameObject>
	FORCEINLINE int32 FindChildren( TArray<T*>& OutChildren, const FString& InTag = TEXT("") ) const 
	{
		TArray<UGameObject*> Objects;
		int32 Num = FindChildren_Internal( T::StaticClass(), InTag, Objects, false );
		ReinterpretObjectArray( Objects, OutChildren );
		return Num;
	}

	/**
	* Find descendants that meets some certain criteria.
	* @param[out]	OutDescendants	The find result
	* @param[in]	InClass			The descendant's class.
	* @param[in]	InTag			The descendant's tag, if this is not specified then any Tag will do.
	* @return the number of descendants that match the criteria.
	*/
	template<class T=UGameObject>
	FORCEINLINE int32 FindDescendants( TArray<T*>& OutDescendants,  TSubclassOf<T> InClass, const FString& InTag = TEXT("") ) const 
	{
		TArray<UGameObject*> Objects;
		int32 Num = FindChildren_Internal( *InClass ? InClass : T::StaticClass(), InTag, Objects, true );
		ReinterpretObjectArray( Objects, OutDescendants );
		return Num;
	}

	/**
	* Find descendants that meets some certain criteria.
	* @param[out]	OutDescendants	The find result
	* @param[in]	InTag			The descendant's tag, if this is not specified then any Tag will do.
	* @return the number of descendants that match the criteria.
	*/
	template<class T=UGameObject>
	FORCEINLINE int32 FindDescendants( TArray<T*>& OutDescendants, const FString& InTag = TEXT("") ) const 
	{
		TArray<UGameObject*> Objects;
		int32 Num = FindChildren_Internal( T::StaticClass(), InTag, Objects, true );
		ReinterpretObjectArray( Objects, OutDescendants );
		return Num;
	}

	/**
	* Add a child to this container.
	* @note Operation will fail If the child to be added is an ancestor of this container.
	* @param	InChild					Child to be added to this container.
	* @param	InChildID				The ID of the child, if it's not specified then the child's default ID will be used, 
	*									if it's also not specified then a unique ID will be generated automatically.
	* @param	bReplaceExistingChild	If InChildID is specified and there is already a child with the same ID,
	*									the existing child will be replaced if this flag is true.
	* @return false if the operation failed.
	*/
	UGameObject* AddChild( UGameObject* InChild, const FString& InChildID = TEXT(""), bool bOverwrite = true );

	/**
	* Create and add a child to this container.
	* @param	InChildclass			The class of the child
	* @param	InChildID				The ID of the child, if it's not specified then the child's default ID will be used, 
	*									if it's also not specified then a unique ID will be generated automatically.
	* @param	bReplaceExistingChild	If InChildID is specified and there is already a child with the same ID,
	*									the existing child will be replaced if this flag is true.
	*/
	template<class T=UGameObject>
	FORCEINLINE T* CreateChild( TSubclassOf<T> InClass, const FString& InID, bool bOverwrite = false )
	{
		return Cast<T>( CreateChild_Internal( *InClass ? InClass : T::StaticClass(), InID, bOverwrite ) );
	}

	/**
	* Create and add a child to this container.
	* @param	InChildID				The ID of the child, if it's not specified then the child's default ID will be used, 
	*									if it's also not specified then a unique ID will be generated automatically.
	* @param	bReplaceExistingChild	If InChildID is specified and there is already a child with the same ID,
	*									the existing child will be replaced if this flag is true.
	*/
	template<class T=UGameObject>
	FORCEINLINE T* CreateChild( const FString& InID, bool bOverwrite = false )
	{
		return Cast<T>( CreateChild_Internal( T::StaticClass(), InID, bOverwrite) );
	}

	/**
	* Get an existing child, if it does not exist then create a new one.
	* @param	InClass		The child's class.
	* @param	InID		The child's ID.
	* @return the child.
	*/
	template<class T=UGameObject>
	FORCEINLINE T* GetOrCreateChild( TSubclassOf<T> InClass, const FString& InID )
	{
		return Cast<T>( GetOrCreateChild_Internal( *InClass ? InClass : T::StaticClass(), InID ) );
	}

	/**
	* Get an existing child, if it does not exist then create a new one.
	* @param	InClass		The child's class.
	* @param	InID		The child's ID.
	* @return the child.
	*/
	template<class T=UGameObject>
	FORCEINLINE T* GetOrCreateChild( const FString& InID )
	{
		return Cast<T>( GetOrCreateChild_Internal( T::StaticClass(), InID ) );
	}


	// UGameObject interface
	virtual void Tick(float DeltaTime) override;
	virtual void SimulationTick(const FTimespan& Timespan) override;
	// End of UGameObject interface

protected:

	typedef TMap<FString, UGameObject*> FGameObjectMap;
	typedef TPair<FString, UGameObject*> FGameObjectPair;

	/** Children of this container. This should not be edited manually, please use the various children manipulation functions. */
	UPROPERTY()
	TMap<FString, UGameObject*> Children;
	
	/** Called after a child has been added to this container. */
	virtual void OnChildAdded(UGameObject* Child) { ReceiveChildAdded(Child); }

	/** Called after a child has been removed from this container. */
	virtual void OnChildRemoved(UGameObject* Child) { ReceiveChildRemoved(Child); }

	/** Event triggered after a new child has been added. */
	UFUNCTION(BlueprintImplementableEvent, Category="GameObjectContainer|Event", meta=(DisplayName="OnChildAdded"))
	void ReceiveChildAdded(UGameObject* Child);

	/** Event triggered after a child has been removed. */
	UFUNCTION(BlueprintImplementableEvent, Category="GameObjectContainer|Event", meta=(DisplayName="OnChildRemoved"))
	void ReceiveChildRemoved(UGameObject* Child);

	// UGameObject protected interface
	virtual void OnAddedToObjectTree(UGameObjectTree* ToObjectTree) override;
	virtual void OnRemovedFromObjectTree(UGameObjectTree* FromObjectTree) override;
	virtual void OnDispose() override;
	// End of UGameObject protected interface

private:

	UGameObject* CreateChild_Internal( UClass* InClass, const FString& InID, bool bOverwrite );

	UGameObject* GetOrCreateChild_Internal( UClass* InClass, const FString& InID );

	UGameObject* FindChild_Internal( UClass* InClass, const FString& InID, const FString& InTag, bool bRecursive ) const;

	int32 FindChildren_Internal( UClass* InClass, const FString& InTag, TArray<UGameObject*>& OutObjects, bool bRecursive ) const;

	void GenerateUniqueID( UObject* Object, FString& OutID );

	bool LockChildren();

	void UnlockChildren();

	bool bIsChildrenLocked;

	bool bIsChildrenDirty;

	UPROPERTY(Transient)
	TArray<UGameObject*> PendingAddedChildren;
};