// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GamePrivatePCH.h"
#include "GameObjectTree.h"
#include "GameUtil.h"

UGameObjectContainer::UGameObjectContainer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsChildrenLocked(false)
	, bIsChildrenDirty(false)
{
}

bool UGameObjectContainer::IsAncestorOf(UGameObject* Other) const
{
	if (!Other)
	{
		return false;
	}

	UGameObjectContainer* Container = Other->Parent.Get();
	while (Container)
	{
		if (Container == this)
		{
			return true;
		}
		Container = Container->Parent.Get();
	}

	return false;
}


UGameObject* UGameObjectContainer::BPF_AddChild( UGameObject* InChild, const FString& InID, bool bOverwrite, ESuccess& OutResult )
{
	UGameObject* Object = AddChild( InChild, InID, bOverwrite );
	OutResult = GetSuccessEnum( Object != nullptr );
	return Object;
}


UGameObject* UGameObjectContainer::BPF_CreateChild( TSubclassOf<UGameObject> InClass, const FString& InID, ESuccess& OutResult )
{
	UGameObject* Object = CreateChild_Internal( InClass, InID, false );
	OutResult = GetSuccessEnum( Object != nullptr );
	return Object;
}


UGameObject* UGameObjectContainer::BPF_GetOrCreateChild( TSubclassOf<UGameObject> InClass, const FString& InID )
{
	return GetOrCreateChild_Internal( InClass, InID );
}

UGameObject* UGameObjectContainer::AddChild( UGameObject* InChild, const FString& InID, bool bOverwrite )
{
	// Check if this new child can actually be added to the child list.
	if ( !InChild 
		|| InChild->IsPendingKill() 
		|| InChild->Parent.Get() == this 
		|| InChild->IsA( UGameObjectTree::StaticClass() ) 
		|| this->IsDescendantOf( Cast<UGameObjectContainer>(InChild) ) )
	{
		return nullptr;
	}

	// Resolve ID, child's ID must be unique in a container.
	FString ChildID = InID;
	if ( ChildID.IsEmpty() )
	{
		// ID is not specified, generate a unique ID.
		GenerateUniqueID( InChild, ChildID );
	}
	else
	{
		// ID is specified, check for ID clash.
		UGameObject** ExistingChild = Children.Find( ChildID );
		if ( ExistingChild )
		{
			// If bOverwrite is true then overwrite the existing object, else failed.
			if ( bOverwrite )
			{
				RemoveChild( *ExistingChild, true );
			}
			else
			{
				Trace("Can not add child because there is already an existing child with the same ID and bOverwrite is false.");
				return nullptr;
			}
		}
	}
	InChild->ID = ChildID;

	// Get the  object tree of the new child, to be used later to determine whether to trigger event related to object tree.
	UGameObjectTree* ChildPrevTree = InChild->GetObjectTree();
	
	// Remove the new child from its previous parent.
	if (UGameObjectContainer* PrevParent = InChild->Parent.Get())
	{
		PrevParent->Children.Remove(InChild->GetID());
		InChild->Parent.Reset();
		PrevParent->OnChildRemoved(InChild);
		InChild->OnRemovedFromParent(PrevParent);
	}

	// Add the new child to this container.
	if ( bIsChildrenLocked )
	{
		PendingAddedChildren.Add( InChild );
		bIsChildrenDirty = true;
	}
	else
	{
		Children.Add( ChildID, InChild );
	}
	InChild->Parent = this;
	OnChildAdded(InChild);
	InChild->OnAddedToParent(this);

	// Trigger added to tree on the new child if necessary.
	UGameObjectTree* ChildCurrentTree = GetObjectTree();
	if ( ChildPrevTree != ChildCurrentTree )
	{
		if ( ChildPrevTree )
		{
			InChild->OnRemovedFromObjectTree( ChildPrevTree );
		}
		if ( ChildCurrentTree )
		{
			InChild->OnAddedToObjectTree( ChildCurrentTree );
		}		
	}


	// Go ahead and return the new child.
	return InChild;
}

void UGameObjectContainer::RemoveChild(UGameObject* InChild, bool bDispose /* = true */)
{
	// Sanity check.
	if ( !InChild || InChild->IsPendingKill() || InChild->Parent.Get() != this )
	{
		return;
	}

	// Remove the child from this container
	if ( bIsChildrenLocked )
	{
		bIsChildrenDirty = true;
	}
	else
	{
		Children.Remove( InChild->ID );
	}	
	InChild->Parent.Reset();
	OnChildRemoved(InChild);
	InChild->OnRemovedFromParent(this);

	// Trigger removed from object tree if this container is attached to object tree.
	if ( UGameObjectTree* ObjectTree = GetObjectTree() )
	{
		InChild->OnRemovedFromObjectTree( ObjectTree );
	}

	// If dispose is specified then dispose the child.
	if (bDispose)
	{
		InChild->Dispose();
	}
}


void UGameObjectContainer::RemoveChildren(bool bDispose /*= true*/)
{
	for ( FGameObjectPair& Pair: Children)
	{
		UGameObject* Child = Pair.Value;

		Child->Parent.Reset();

		OnChildRemoved(Child);
		Child->OnRemovedFromParent(this);

		if (bDispose)
		{
			Child->Dispose();
		}
	}

	if ( bIsChildrenLocked )
	{
		bIsChildrenDirty = true;
	}
	else
	{
		Children.Empty();
	}
}

void UGameObjectContainer::GetChildren( TArray<UGameObject*>& OutChildren ) const
{
	FindChildren_Internal( nullptr, TEXT(""), OutChildren, false );
}


UGameObject* UGameObjectContainer::BPF_FindChild( TSubclassOf<UGameObject> InClass, const FString& InID, const FString& InTag ) const
{
	return FindChild_Internal( InClass, InID, InTag, false );
}


UGameObject* UGameObjectContainer::BPF_FindChildEx( TSubclassOf<UGameObject> InClass, const FString& InID, const FString& InTag, EFindResult& OutResult ) const
{
	UGameObject* Child = FindChild_Internal( InClass, InID, InTag, false );
	OutResult = GetFindResultEnum( Child != nullptr );
	return Child;
}


void UGameObjectContainer::BPF_FindChildren( TSubclassOf<UGameObject> InClass, const FString& InTag, TArray<UGameObject*>& OutChildren ) const
{
	FindChildren_Internal( InClass, InTag, OutChildren, false );
}


void UGameObjectContainer::BPF_FindChildrenEx( TSubclassOf<UGameObject> InClass, const FString& InTag, TArray<UGameObject*>& OutChildren, EFindResult& OutResult ) const
{
	int32 Num = FindChildren_Internal( InClass, InTag, OutChildren, false );
	OutResult = GetFindResultEnum( Num > 0 );
}


void UGameObjectContainer::GetDescendants( TArray<UGameObject*>& OutDescendants ) const
{
	FindChildren_Internal( nullptr, TEXT(""), OutDescendants, true );
}


UGameObject* UGameObjectContainer::BPF_FindDescendant( TSubclassOf<UGameObject> InClass, const FString& InID, const FString& InTag ) const
{
	return FindChild_Internal( InClass, InID, InTag, true );
}


UGameObject* UGameObjectContainer::BPF_FindDescendantEx( TSubclassOf<UGameObject> InClass, const FString& InID, const FString& InTag, EFindResult& OutResult ) const
{
	UGameObject* Object = FindChild_Internal( InClass, InID, InTag, true );
	OutResult = GetFindResultEnum( Object != nullptr );
	return Object;
}


void UGameObjectContainer::BPF_FindDescendants( TSubclassOf<UGameObject> InClass, const FString& InTag, TArray<UGameObject*>& OutDescendants ) const
{
	FindChildren_Internal( InClass, InTag, OutDescendants, true );
}


void UGameObjectContainer::BPF_FindDescendantsEx( TSubclassOf<UGameObject> InClass, const FString& InTag, TArray<UGameObject*>& OutDescendants, EFindResult& OutResult ) const
{
	int32 Num = FindChildren_Internal( InClass, InTag, OutDescendants, true );
	OutResult = GetFindResultEnum( Num > 0 );
}

void UGameObjectContainer::Tick(float DeltaTime)
{
	Super::Tick( DeltaTime );

	if (bAllowChildrenToTick)
	{
		bool bLockingChildren = LockChildren();
		
		for (FGameObjectPair& Pair : Children )
		{
			UGameObject* Child = Pair.Value;
			if ( Child->bCanTick && Child->GetParent() == this && Child->IsPendingKill() == false )
			{
				Child->Tick(DeltaTime);
			}
		}

		if ( bLockingChildren )
		{
			UnlockChildren();
		}
	}
}

void UGameObjectContainer::SimulationTick(const FTimespan& Timespan)
{
	Super::SimulationTick( Timespan );

	if (bAllowChildrenToTick)
	{
		bool bLockingChildren = LockChildren();

		for ( FGameObjectPair& Pair : Children )
		{
			UGameObject* Child = Pair.Value;
			if ( Child->bCanSimulationTick && Child->GetParent() == this && Child->IsPendingKill() == false )
			{
				Child->SimulationTick(Timespan);
			}
		}

		if ( bLockingChildren )
		{
			UnlockChildren();
		}
	}
}


void UGameObjectContainer::OnAddedToObjectTree( UGameObjectTree* ToObjectTree )
{
	Super::OnAddedToObjectTree( ToObjectTree );

	bool bLockingChildren = LockChildren();

	for ( FGameObjectPair& Pair : Children )
	{
		UGameObject* Child = Pair.Value;
		if ( Child->GetParent() == this && Child->IsPendingKill() == false )
		{
			Child->OnAddedToObjectTree( ToObjectTree );
		}
	}

	for ( UGameObject* Child : PendingAddedChildren )
	{
		if ( Child->GetParent() == this && Child->IsPendingKill() == false )
		{
			Child->OnAddedToObjectTree( ToObjectTree );
		}
	}

	if ( bLockingChildren )
	{
		UnlockChildren();
	}
}


void UGameObjectContainer::OnRemovedFromObjectTree( UGameObjectTree* FromObjectTree )
{
	Super::OnRemovedFromObjectTree( FromObjectTree );

	bool bLockingChildren = LockChildren();

	for ( FGameObjectPair& Pair : Children )
	{
		UGameObject* Child = Pair.Value;
		if ( Child->GetParent() == this && Child->IsPendingKill() == false )
		{
			Child->OnRemovedFromObjectTree( FromObjectTree );
		}
	}

	for ( UGameObject* Child : PendingAddedChildren )
	{
		if ( Child->GetParent() == this && Child->IsPendingKill() == false )
		{
			Child->OnRemovedFromObjectTree( FromObjectTree );
		}
	}

	if ( bLockingChildren )
	{
		UnlockChildren();
	}
}

void UGameObjectContainer::OnDispose()
{
	Super::OnDispose();

	for ( FGameObjectPair& Pair : Children )
	{
		UGameObject* Child = Pair.Value;
		if ( Child->GetParent() == this )
		{
			Child->Parent.Reset();
			Child->Dispose();
		}
	}
	Children.Empty();	

	for ( UGameObject* Child : PendingAddedChildren )
	{
		if ( Child->GetParent() == this )
		{
			Child->Parent.Reset();
			Child->Dispose();
		}
	}
	PendingAddedChildren.Empty();
}

UGameObject* UGameObjectContainer::CreateChild_Internal( UClass* InClass, const FString& InID, bool bOverwrite )
{
	if ( !InClass )
	{
		return nullptr;
	}

	UGameObject* Object = CreateGameObject<UGameObject>( InClass );
	
	if ( AddChild( Object, InID, bOverwrite ) == nullptr )
	{
		Object->Dispose();
		Object = nullptr;
	}
	
	return Object;
}

UGameObject* UGameObjectContainer::GetOrCreateChild_Internal( UClass* InClass, const FString& InID )
{
	if ( !InClass )
	{
		return nullptr;
	}

	UGameObject* Object = FindChild_Internal( InClass, InID, TEXT(""), false );
	if ( !Object )
	{
		Object = CreateChild_Internal( InClass, InID, true );
	}
	return Object;	
}

UGameObject* UGameObjectContainer::FindChild_Internal( UClass* InClass, const FString& InID, const FString& InTag, bool bRecursive ) const
{
	if ( Children.Num() == 0 )
	{
		return nullptr;
	}

	bool bIsClassSpecified = ( InClass != nullptr ) || (InClass == UGameObject::StaticClass());
	bool bIsIDSpecified = !InID.IsEmpty();
	bool bIsTagSpecified = !InTag.IsEmpty();

	if ( !bIsClassSpecified && !bIsIDSpecified && !bIsTagSpecified )
	{
		UGameObject* Object = Children.CreateConstIterator().Value();
		return Object;
	}
	else if ( bIsIDSpecified )
	{
		UGameObject * const * pObject = Children.Find( InID );
		if ( !pObject )
		{
			return nullptr;
		}
		UGameObject* Object = *pObject;
		if ( !Object 
			 || ( bIsClassSpecified && Object->IsA(InClass) == false )
			 || ( bIsTagSpecified && Object->HasTag( InTag ) == false ) )
		{
			return nullptr;
		}
		return Object;
	}
	else 
	{
		for ( const FGameObjectPair& Pair : Children )
		{
			UGameObject* Object = Pair.Value;
			if ( Object->GetParent() != this || Object->IsPendingKill() )
			{
				continue;
			}

			if ( bIsClassSpecified && Object->IsA( InClass ) == false )
			{
				continue;
			}
			if ( bIsTagSpecified && Object->HasTag(InTag) == false )
			{
				continue;
			}
			return Object;
		}

		for ( UGameObject* Object : PendingAddedChildren )
		{
			if ( Object->GetParent() != this || Object->IsPendingKill() )
			{
				continue;
			}
			if ( bIsClassSpecified && Object->IsA( InClass ) == false )
			{
				continue;
			}
			if ( bIsTagSpecified && Object->HasTag(InTag) == false )
			{
				continue;
			}
			return Object;
		}
	}

	// At this point, there is no immediate child that meets the criteria, 
	// if recursive is not specified then this is the end.
	if ( bRecursive == false )
	{
		return nullptr;
	}

	// Recursive
	for ( const FGameObjectPair& Pair : Children )
	{
		if ( UGameObjectContainer* Container = Cast<UGameObjectContainer>( Pair.Value ) )
		{
			if ( Container->GetParent() != this || Container->IsPendingKill() )
			{
				continue;
			}
			if ( UGameObject* Object = Container->FindChild_Internal(InClass, InID, InTag, true ) )
			{
				return Object;
			}
		}
	}
	for ( UGameObject* PendingObject : PendingAddedChildren )
	{
		if ( UGameObjectContainer* Container = Cast<UGameObjectContainer>( PendingObject ) )
		{
			if ( Container->GetParent() != this || Container->IsPendingKill() )
			{
				continue;
			}
			if ( UGameObject* Object = Container->FindChild_Internal(InClass, InID, InTag, true ) )
			{
				return Object;
			}
		}
	}

	// Nope, nothing ...
	return nullptr;
}

int32 UGameObjectContainer::FindChildren_Internal( UClass* InClass, const FString& InTag, TArray<UGameObject*>& OutObjects, bool bRecursive ) const
{
	OutObjects.Empty();

	if ( Children.Num() == 0 )
	{
		return 0;
	}

	bool bIsClassSpecified = ( InClass != nullptr ) || (InClass == UGameObject::StaticClass());
	bool bIsTagSpecified = !InTag.IsEmpty();

	if ( !bIsClassSpecified && !bIsTagSpecified )
	{
		Children.GenerateValueArray( OutObjects );
	}
	else 
	{
		for ( const FGameObjectPair& Pair : Children )
		{
			UGameObject* Object = Pair.Value;
			if ( Object->GetParent() != this || Object->IsPendingKill() )
			{
				continue;
			}

			if ( bIsClassSpecified && Object->IsA( InClass ) == false )
			{
				continue;
			}
			if ( bIsTagSpecified && Object->HasTag(InTag) == false )
			{
				continue;
			}
			OutObjects.Add( Object );
		}

		for ( UGameObject* Object : PendingAddedChildren )
		{
			if ( Object->GetParent() != this || Object->IsPendingKill() )
			{
				continue;
			}
			if ( bIsClassSpecified && Object->IsA( InClass ) == false )
			{
				continue;
			}
			if ( bIsTagSpecified && Object->HasTag(InTag) == false )
			{
				continue;
			}
			OutObjects.Add( Object );
		}
	}

	// if recursive is not specified then this is the end.
	if ( bRecursive == false )
	{
		return OutObjects.Num();
	}

	// Recursive
	for ( const FGameObjectPair& Pair : Children )
	{
		if ( UGameObjectContainer* Container = Cast<UGameObjectContainer>( Pair.Value ) )
		{
			if ( Container->GetParent() != this || Container->IsPendingKill() )
			{
				continue;
			}
			TArray<UGameObject*> Objects;
			Container->FindChildren_Internal( InClass, InTag, Objects, true );
			OutObjects.Append( Objects );
		}
	}
	for ( UGameObject* PendingObject : PendingAddedChildren )
	{
		if ( UGameObjectContainer* Container = Cast<UGameObjectContainer>( PendingObject ) )
		{
			if ( Container->GetParent() != this || Container->IsPendingKill() )
			{
				continue;
			}
			TArray<UGameObject*> Objects;
			Container->FindChildren_Internal( InClass, InTag, Objects, true );
			OutObjects.Append( Objects );
		}
	}

	// Done
	return OutObjects.Num();
}

void UGameObjectContainer::GenerateUniqueID( UObject* Object, FString& OutID )
{
	FString BaseID;
	Object->GetName( BaseID );

	int32 Index = 0;
	do 
	{
		OutID = FString::Printf( TEXT("%s_%d"), *BaseID, Index );
		Index++;
	}
	while( Children.Contains(OutID) );
}

bool UGameObjectContainer::LockChildren()
{
	if ( bIsChildrenLocked )
	{
		return false;
	}
	bIsChildrenLocked = true;
	return true;
}

void UGameObjectContainer::UnlockChildren()
{
	if ( !bIsChildrenLocked )
	{
		return;
	}

	bIsChildrenLocked = false;

	if ( bIsChildrenDirty )
	{
		FGameObjectMap CleanedMap;

		for ( const FGameObjectPair& Pair : Children )
		{
			UGameObject* Object = Pair.Value;
			if ( Object && Object->GetParent() == this && Object->IsPendingKill() == false )
			{
				CleanedMap.Add( Object->ID, Object );
			}
		}

		for ( UGameObject* Object : PendingAddedChildren )
		{
			if ( Object && Object->GetParent() == this && Object->IsPendingKill() == false )
			{
				CleanedMap.Add( Object->ID, Object );
			}
		}
		PendingAddedChildren.Empty();

		Children = CleanedMap;

		bIsChildrenDirty = false;
	}
}