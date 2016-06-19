// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GamePrivatePCH.h"
#include "Object/GameObjectTree.h"
#include "Framework/Game.h"

UGameObjectTree::UGameObjectTree(const FObjectInitializer& ObjectInitializer)
	: Super( ObjectInitializer )
{
	bAllowChildrenToTick = true;
	bCanTick = bCanSimulationTick = true;
}


UGameObjectContainer* UGameObjectTree::GetOrCreateContainerForObject( TSubclassOf<UGameObject> InClass )
{
	if ( !InClass )
	{
		return nullptr;
	}

	FString ContainerID = FString::Printf( TEXT("GenericContainer_%s"), *InClass->GetName() );
	
	UGameObjectContainer* Container = FindChild<UGameObjectContainer>( ContainerID );
	if ( !Container )
	{
		Container = CreateChild<UGameObjectContainer>( ContainerID );
		
		UGameObject* CDO = InClass->GetDefaultObject<UGameObject>();
		if ( !CDO->bCanTick && !CDO->bCanSimulationTick )
		{
			Container->bAllowChildrenToTick = false;
		}
		else
		{
			Container->bCanTick = CDO->bCanTick;
			Container->bCanSimulationTick = CDO->bCanSimulationTick;
		}
	}

	return Container;
}

bool UGameObjectTree::SaveToRecord( FGameObjectTreeRecord& OutTreeRecord ) const
{
	Trace( "Begin Saving ..." );

	OutTreeRecord.ObjectRecords.Empty();
	OutTreeRecord.LinkRecords.Empty();
	OutTreeRecord.ByteData.Empty();

	TArray<UGameObject*> Objects;
	GetDescendants( Objects );

	FGameObjectLinkRecord MyLinkRecord;
	MyLinkRecord.ParentName = NAME_None;

	for (UGameObject* Object : Objects)
	{
		if (!Object)
		{
			continue;
		}

		FStringAssetReference ObjectRef = Object;

		// Create Object record
		FGameObjectRecord ObjectRecord;
		ObjectRecord.Class = Object->GetClass();
		FName ObjectName = ObjectRecord.Name = Object->GetFName();
		ObjectRecord.ID = Object->ID;

		// Serialize Object data
		ObjectRecord.ByteData.Empty();
		FMemoryWriter Writer(ObjectRecord.ByteData);
		FSaveGameArchive Ar(Writer);
		Object->Serialize(Ar);
		Trace("    Save Object: %s", *GetFullNameSafe(Object));

		// Add it to tree record.
		OutTreeRecord.ObjectRecords.Add(ObjectRecord);

		// If this object is a tree object direct children, record it.
		if (Object->GetParent() == this)
		{
			MyLinkRecord.ChildNames.Add( ObjectName );
			Trace("   Link Parent( %s ) -> Child( %s )", *GetFullName(), *Object->GetFullName() );
		}

		// If this object is a container, then record its children references.
		UGameObjectContainer* Container = Cast<UGameObjectContainer>(Object);
		if (Container)
		{
			FGameObjectLinkRecord LinkRecord;
			LinkRecord.ParentName = ObjectName;
			for (const FGameObjectPair& Pair : Container->Children)
			{
				UGameObject* Child = Pair.Value;
				if (Child)
				{
					LinkRecord.ChildNames.Add( Child->GetFName() );
					Trace("   Link Parent( %s ) -> Child( %s )", *Container->GetFullName(), *Child->GetFullName() );
				}
			}
			OutTreeRecord.LinkRecords.Add(LinkRecord);
		}
	}

	// Record my Links
	OutTreeRecord.LinkRecords.Add(MyLinkRecord);

	// Serialize my data
	FMemoryWriter Writer(OutTreeRecord.ByteData);
	FSaveGameArchive Ar(Writer);
	UObject* Self = (UObject*) this;
	Self->Serialize(Ar);

	Trace("Saving ended.");
	return true;

}

static UGameObject* FindGameObjectInMap(const TMap<UGameObject*, const FGameObjectRecord*>& ObjectMap, const FName& Name)
{
	if (Name.IsNone())
	{
		return nullptr;
	}
	for (const auto& i : ObjectMap)
	{
		 UGameObject* Object = (UGameObject*) i.Key;
		if (Object && Object->GetFName() == Name)
			return Object;
	}
	return nullptr;
}

bool UGameObjectTree::LoadFromRecord(const FGameObjectTreeRecord& InTreeRecord)
{
	UObject* Outer = (UObject*) GetTransientPackage();

	bool bHasError = false;

	RemoveChildren( true );

	TMap<UGameObject*, const FGameObjectRecord*> ObjectMap;

	// Recreate game objects
	for (const FGameObjectRecord& ObjectRecord : InTreeRecord.ObjectRecords)
	{
		// Make sure record is valid
		if (!ObjectRecord.Class || ObjectRecord.Name.IsNone())
		{
			PrintLogError( "Corrupt Object's Record: Class='%s', Name='%s'", *GetFullNameSafe(ObjectRecord.Class), *ObjectRecord.Name.ToString() );
			bHasError = true;
			continue;
		}

		// Create object
		UGameObject* Object = NewObject<UGameObject>( Outer, ObjectRecord.Class, ObjectRecord.Name);
		if (!Object)
		{
			PrintLogError( "Fail to create Object: Class='%s', Name='%s'" , *GetFullNameSafe(ObjectRecord.Class), *ObjectRecord.Name.ToString() );
			bHasError = true;
			continue;
		}

		// retrieve ID
		Object->ID = ObjectRecord.ID;

		// Make sure it's not garbage collected
		Object->AddToRoot();		

		// Add it to map for the last loading phase of data serialization.
		ObjectMap.Add( Object, &ObjectRecord );
		Trace( "Recreate Object : %s", *Object->GetFullName() );
	}


	// Rebuild Links
	for (const FGameObjectLinkRecord& LinkRecord : InTreeRecord.LinkRecords)
	{
		UGameObjectContainer* Parent;
		if ( LinkRecord.ParentName.IsNone() )
		{
			Parent = Cast<UGameObjectContainer>( this );
		}
		else
		{
			Parent = Cast<UGameObjectContainer>( FindGameObjectInMap(ObjectMap, LinkRecord.ParentName) );
		}

		if ( !Parent )
		{
			PrintLogError( "Parent is not a container" );
			bHasError = true;
			continue;
		}

		for ( const FName& ChildName : LinkRecord.ChildNames )	
		{
			UGameObject* Child = Cast<UGameObject>( FindGameObjectInMap(ObjectMap, ChildName) );
			if ( !Child )
			{
				PrintLogError( "Child is not a game object" );
				bHasError = true;
				continue;
			}

			Child->Parent = Parent;
			Parent->Children.Add( Child->ID, Child );
			Trace( "[%s] Parent: (%s) -> [%s] Child (%s)", *Parent->ID, *Parent->GetPathName(), *Child->ID, *Child->GetPathName() );
		}
	}

	// Serialize objects
	for (const TPair<UGameObject*, const FGameObjectRecord*>& Pair : ObjectMap)
	{
		UGameObject* Object = Pair.Key;
		const TArray<uint8>& ByteData = Pair.Value->ByteData;

		FMemoryReader MemoryReader(ByteData);
		FSaveGameArchive Ar(MemoryReader);
		Object->Serialize(Ar);

		// It's now okay to remove it from ''root''.
		Object->RemoveFromRoot();
	}

	// Serialize self
	FMemoryReader MemoryReader(InTreeRecord.ByteData);
	FSaveGameArchive Ar(MemoryReader);
	this->Serialize(Ar);


	return bHasError == false;
}

