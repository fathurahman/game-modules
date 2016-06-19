// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GamePrivatePCH.h"
#include "Object/ObjectWithTags.h"

bool UObjectWithTags::HasTag(FString Tag) const
{
	return Tags.Contains(Tag);
}

bool UObjectWithTags::HasAnyTags(const TArray<FString>& InTags) const
{
	for (const FString& Tag : InTags)
	{
		if (Tags.Contains(Tag))
		{
			return true;
		}
	}
	return false;
}

bool UObjectWithTags::HasAllTags(const TArray<FString>& InTags) const
{
	for (const FString& Tag : InTags)
	{
		if (Tags.Contains(Tag) == false)
		{
			return false;
		}
	}
	return true;
}

void UObjectWithTags::AddTag(const FString& InTag)
{
	if ( Tags.Contains(InTag) == false )
	{
		Tags.Add( InTag );
	}
}

void UObjectWithTags::RemoveTag(const FString& InTag)
{
	Tags.Remove( InTag );
}

