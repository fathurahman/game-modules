// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "GameObjectContainer.h"
#include "GameData.generated.h"

class UGameDataInstance;

/** UGameData.
* Static Data Object, Object that usually never change throughout the game and can be instanced into an asset.
*/
UCLASS(Abstract, Blueprintable)
class GAME_API UGameData : public UObject
{
	GENERATED_BODY()
public:	


protected:

	UPROPERTY(EditDefaultsOnly, Category="GameData")
	TSubclassOf<class UGameObject> InstanceClass;

	/** Get the instance of this game data. */
	UFUNCTION(BlueprintPure, Category="GameData", meta=(DisplayName="GetInstance"))
	UGameObject* BPF_GetInstance() { return GetInstance(); }

	/** Event triggered after an instance of this game data has been created. */
	UFUNCTION(BlueprintImplementableEvent, Category="GameData", meta=(DisplayName="OnInitInstance"))
	void BPF_OnInitInstance( UGameObject* Instance );

	/** Get the instance of this game data. */
	UGameObject* GetInstance();

	/** Get a typed instance of this game data. */
	template<class T>
	FORCEINLINE T* GetInstance() 
	{
		return Cast<T>( GetInstance() );
	}

	/** This function is called after an instance has been created. */
	virtual void OnInitInstance() {}

private:
	
	TWeakObjectPtr<UGameObject> Instance;
	
};