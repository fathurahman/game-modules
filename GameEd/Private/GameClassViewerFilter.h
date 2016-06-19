// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "ClassViewerFilter.h"

class FGClassViewerFilter: public IClassViewerFilter
{
public:

	FGClassViewerFilter() 
	{
		DisallowedClassFlags = 0;
		bAllowBlueprintClasses = true;
	}

	TSet<const UClass*> AllowedChildrenOfClasses;
	
	uint32 DisallowedClassFlags;

	bool bAllowBlueprintClasses;

	///

	virtual bool IsClassAllowed(
		const FClassViewerInitializationOptions& InInitOptions, 
		const UClass* InClass, 
		TSharedRef<FClassViewerFilterFuncs> InFilterFuncs ) override
	{
		return ( InClass->HasAnyClassFlags(DisallowedClassFlags) == false )
			&& ( InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed );
	}

	virtual bool IsUnloadedClassAllowed(
		const FClassViewerInitializationOptions& InInitOptions, 
		const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData, 
		TSharedRef<FClassViewerFilterFuncs> InFilterFuncs ) override
	{
		return bAllowBlueprintClasses
			&& ( InUnloadedClassData->HasAnyClassFlags(DisallowedClassFlags) == false )
			&& ( InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed );

	}
};

