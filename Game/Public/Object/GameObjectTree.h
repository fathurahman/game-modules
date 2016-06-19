// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "GameTypes.h"
#include "GameObjectContainer.h"
#include "GameObjectTree.generated.h"

/**
* Game Object Tree.
* A special Game Object that can not have a parent and have saving and loading function to save and restore the object tree.
*/
UCLASS(BlueprintType, NotBlueprintable)
class GAME_API UGameObjectTree : public UGameObjectContainer
{
	GENERATED_BODY()

public:

	UGameObjectTree( const FObjectInitializer& ObjectInitializer );

	// UGameObjectContainer interface
	virtual UGameObjectContainer* GetParent() const override final { return nullptr; }
	virtual bool SetParent( UGameObjectContainer* InParent ) override final { return false; }
	virtual UGameObjectTree* GetObjectTree() const override final { return (UGameObjectTree*) this; }
	// End of UGameObjectContainer interface

	UFUNCTION(BlueprintPure, Category="GameObjectTree")
	UGameObjectContainer* GetOrCreateContainerForObject( TSubclassOf<UGameObject> InClass );
	
	template<class T=UGameObject>
	FORCEINLINE T* GetSingletonGameObject( T** ppObject = nullptr )
	{
		T* pObject = ppObject ? (*ppObject) : nullptr;
		if ( !pObject )
		{
			pObject = FindChild<T>();
			if ( !pObject )
			{
				pObject = CreateChild<T>();
			}
		}
		return pObject;
	}

	/**
	 * Load this GameObjectTree from a record.
	 * @param InRecord	Record where this object tree will be loaded from.
	 * @return false if there's an error while loading the tree.
	 */
	bool LoadFromRecord( const FGameObjectTreeRecord& InRecord );

	/**
	 * Save this GameObjectTree to a record. 
	 * @param OutRecord		Record where this object tree will be saved into.
	 * @return false if there's an error while saving the tree.
	 */
	bool SaveToRecord( FGameObjectTreeRecord& OutRecord ) const;
			
};