// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GameUIPrivatePCH.h"
#include "Components/PanelWidget.h"
#include "GUIPlayerController.h"
#include "GUIScreen.h"

UGUIScreen::UGUIScreen(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if ( OpenCloseTween )
	{
		if ( HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject) )
		{
			OpenCloseTween->GetParams().bInitialValue = false;
		}
		else
		{
			OpenCloseTween->OnValueChanged.AddUObject( this, &UGUIScreen::HandleOnOpenClose );
			OpenCloseTween->OnFinished.AddUObject( this, &UGUIScreen::HandleOnOpenCloseFinished );
		}
	}
}

bool UGUIScreen::GetIsActive() const
{
	return bIsActive;
}

void UGUIScreen::Open()
{
	ESuccess Result;
	GetGUIPlayerController()->OpenScreen(this, Result);
}

void UGUIScreen::Close(FString Result)
{
	CloseResult = Result;
	GetGUIPlayerController()->CloseScreen(this, Result);
}


void UGUIScreen::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGUIScreen::SetIsActive(bool bFlag)
{
	if (bIsActive != bFlag)
	{
		bIsActive = bFlag;
		if (bIsActive)
		{
			NativeOnActive();
		}
		else
		{
			NativeOnInactive();
		}
	}
}

void UGUIScreen::NativeOnActive()
{
	OnActive();
}

void UGUIScreen::NativeOnInactive()
{
	OnInactive();
}


void UGUIScreen::HandleOnOpenCloseFinished(bool bIsOpen)
{
	Trace("Handle Open Close %s", bIsOpen ? TEXT("Open") : TEXT("Close"));
	if ( GetOutermost() != GetTransientPackage() )
	{
		return;
	}
	if (bIsOpen == false)
	{
		if (IsInViewport())
		{
			RemoveFromViewport();
		}
	}
}

void UGUIScreen::NativeOpen()
{
	Super::Open();
}

void UGUIScreen::NativeClose(FString Result)
{
	Super::Close(Result);
}

void UGUIScreen::HandleOnOpenClose(bool bIsOpen)
{
	if ( GetOutermost() != GetTransientPackage() )
	{
		return;
	}

	if ( bIsOpen )
	{
		if (IsInViewport() == false)
		{
			AddToViewport(ZOrder);
		}
		OnOpenScreen.Broadcast();
	}
	else 
	{
		OnCloseScreen.Broadcast(CloseResult);
	}
}

