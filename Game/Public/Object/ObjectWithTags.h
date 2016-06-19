// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "ObjectWithTags.generated.h"

/** Object that has tags.
* This is a base class for many game objects, tags can also be added to actor by adding @GameTagsComponent 
* All tags are registered in @GameSingleton
*/
UCLASS(Blueprintable)
class GAME_API UObjectWithTags : public UObject
{
	GENERATED_BODY()
public:

	/**
	* Check if this object has a specific tag.
	* @param 	Tag 	Tag to check.
	* @return true if this object has that specific tag
	*/
	UFUNCTION(BlueprintPure, Category="Tag")
	bool HasTag( FString InTag ) const;	

	/** 
	* Check if this object has any of the Tags 
	* @param 	InTags	Tags to check
	* @return	true if this object has any of the checked Tags
	*/
	UFUNCTION(BlueprintPure, Category="Tag")
	bool HasAnyTags( UPARAM(Ref) const TArray<FString>& InTags ) const;

	/**
	* Check if this object has all of the Tags
	* @param	InTags	Tags to check
	* @return	true if this object has all of the checked Tags
	*/
	UFUNCTION(BlueprintPure, Category="Tag")
	bool HasAllTags( UPARAM(Ref) const TArray<FString>& InTags ) const;

	/**
	* Add a tag to this object.
	* @param InTag 	Tag to add
	*/
	UFUNCTION(BlueprintCallable, Category="Tag")
	void AddTag( UPARAM(Ref) const FString& InTag );

	/**
	* Remove tag from this object.
	* @param InTag 	Tag to remove.
	*/
	UFUNCTION(BlueprintCallable, Category="Tag")
	void RemoveTag( UPARAM(Ref) const FString& InTag );

protected:

	UPROPERTY(EditAnywhere, Category="Tag")
	TArray<FString> Tags;
	
};