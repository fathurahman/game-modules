// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "ObjectWithTags.h"
#include "GameTagsComponent.generated.h"

UCLASS(meta=(BlueprintSpawnableComponent))
class UGameTagsComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UGameTagsComponent( const FObjectInitializer& ObjectInitializer )
		: Super( ObjectInitializer )
	{
		Tags = ObjectInitializer.CreateDefaultSubobject<UObjectWithTags>(this, "ObjectWithTagsDefault");
	}

	UFUNCTION(BlueprintPure, Category="Tag")
	UObjectWithTags* GetTags() const { return Tags; }

protected:

	UPROPERTY(VisibleAnywhere, Instanced, Category="Tag")
	UObjectWithTags* Tags;

};