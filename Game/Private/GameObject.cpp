// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GamePrivatePCH.h"
#include "GameObjectTree.h"

FString UGameObject::GetID() const
{
	return ID;
}

bool UGameObject::IsDescendantOf(UGameObjectContainer* InAncestor) const
{
	return InAncestor ? InAncestor->IsAncestorOf((UGameObject*)this) : false;
}

UGameObjectContainer* UGameObject::GetParent() const
{
	return Parent.Get();
}

bool UGameObject::SetParent(UGameObjectContainer* InParent)
{
	if (!InParent)
	{
		return false;
	}
	return InParent->AddChild(this) != nullptr;
}


UGameObjectTree* UGameObject::GetObjectTree() const
{
	UGameObjectContainer* Container = Parent.Get();
	while ( Container && Container->Parent.IsValid() )
	{
		Container = Container->Parent.Get();
	}
	return Container ? Cast<UGameObjectTree>( Container ) : nullptr;
}

void UGameObject::Tick(float DeltaTime)
{
	ReceiveTick( DeltaTime );
}

void UGameObject::SimulationTick(const FTimespan& Timespan)
{
	ReceiveSimulationTick( Timespan );
}

void UGameObject::Dispose()
{
	if (IsPendingKill() == false)
	{
		if (Parent.IsValid())
		{
			Parent->RemoveChild(this, false);
		}

		OnDispose();

		MarkPendingKill();
	}
}


FString UGameObject::GetIDOrName() const
{
	return ID.IsEmpty() ? GetName() : ID;
}

UWorld* UGameObject::GetWorld() const
{
	return GWorld.GetReference();
}

