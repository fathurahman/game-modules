// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GameUIPrivatePCH.h"
#include "Components/PanelWidget.h"
#include "GUIPlayerController.h"
#include "GUIWidget.h"

UGUIWidget::UGUIWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OpenCloseTween = ObjectInitializer.CreateDefaultSubobject<UTweeningBoolean>(this, "OpenCloseTweenDefault");
	if ( OpenCloseTween )
	{
		if ( HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject) )
		{
			FTweeningBooleanParameters Params;
			Params.TweenParamsIn = FTweenParameters(EEaseType::Linear, 0.f);
			Params.TweenParamsOut = FTweenParameters(EEaseType::Linear, 0.f);
			Params.bInitialValue = true;
			OpenCloseTween->SetParams( Params );
		}
		else
		{
			//OpenCloseTween->OnValueChanged.AddUObject( this, &UGUIWidget::OnOpenClose );
			OpenCloseTween->OnTweening.AddUObject( this, &UGUIWidget::OnOpenCloseTweening );
			OpenCloseTween->OnFinished.AddUObject( this, &UGUIWidget::OnOpenCloseFinished );
		}
	}

	Tweener = ObjectInitializer.CreateDefaultSubobject<UTweener>(this, "TweenerDefault");
	BooleanTweener = ObjectInitializer.CreateDefaultSubobject<UBooleanTweener>(this, "BooleanTweenerDefault");
	StateMachine = ObjectInitializer.CreateDefaultSubobject<UTweeningStateMachine>(this, "StateMachineDefault");
}

AGUIPlayerController* UGUIWidget::GetGUIPlayerController() const
{
	return CastChecked<AGUIPlayerController>(GetOwningPlayer());
}

bool UGUIWidget::SetHasInputKeyFocus(bool bFlag)
{
	if (bFlag != bHasInputKeyFocus)
	{
		if (bFlag)
		{
			if (CanReceiveInputKey() == false)
			{
				return false;
			}
		}

		bHasInputKeyFocus = bFlag;

		if (bHasInputKeyFocus)
		{
			NativeOnInputKeyFocusReceived();
		}
		else
		{
			NativeOnInputKeyFocusLost();
		}

		return true;
	}

	return false;
}

bool UGUIWidget::GetIsOpen() const
{
	return OpenCloseTween->GetValue();
}

FString UGUIWidget::GetResultUponClose() const
{
	return ResultUponClose;
}

UGUIWidget* UGUIWidget::GetGUIWidgetParent() const
{
	UPanelWidget* Panel = GetParent();
	while (Panel)
	{
		UPanelWidget* p = Panel->GetParent();
		if (p)
		{
			// Still has parent panel, move further up.
			Panel = p;
		}
		else
		{
			// Reached root panel, returns the GUIWidget who owned this root panel.
			return Panel->GetTypedOuter<UGUIWidget>();
		}
	}

	// This widget is not attached to another widget.
	return nullptr;
}

bool UGUIWidget::CanReceiveInputKey() const
{
	return bAllowInputKey 
		&& GetIsEnabled() 
		&& GetIsOpen()
		&& GetVisibility() != ESlateVisibility::Hidden 
		&& GetVisibility() != ESlateVisibility::Collapsed;
}

bool UGUIWidget::GetAllowInputKey() const
{
	return bAllowInputKey;
}

void UGUIWidget::SetAllowInputKey(bool bFlag)
{
	if (bAllowInputKey != bFlag)
	{
		bAllowInputKey = bFlag;
		NativeOnAllowInputKeyChanged(bFlag);
	}
}

bool UGUIWidget::HasInputKeyFocus() const
{
	return bHasInputKeyFocus;
}

bool UGUIWidget::RequestInputKeyFocusFromParent()
{
	if (CanReceiveInputKey() == false)
	{
		UE_LOG(LogGameUI, Warning, TEXT("Widget that can not receive input key focus is trying to request input key focus from its parent"));
		return false;
	}

	UGUIWidget* Parent = GetGUIWidgetParent();
	if (Parent)
	{
		return Parent->SetChildThatHasInputKeyFocus(this);
	}
	return false;
}

UGUIWidget* UGUIWidget::GetChildThatHasInputKeyFocus() const
{
	return ChildThatHasInputKeyFocus.Get();
}

bool UGUIWidget::SetChildThatHasInputKeyFocus(UGUIWidget* ChildWidget)
{
	if (ChildWidget)
	{
		if (ChildWidget->GetGUIWidgetParent() != this)
		{
			// The parameter is not a child of this widget.
			UE_LOG(LogGameUI, Warning, TEXT("Can not set input key focus to non child GUIWidget"));
			return false;
		}
		else if (ChildWidget->CanReceiveInputKey() == false)
		{
			// the child widget can not receive input key focus
			UE_LOG(LogGameUI, Log, TEXT("Child widget can not receive input key focus"));
			return false;
		}
	}

	UGUIWidget* PrevFocus = ChildThatHasInputKeyFocus.Get();

	if (ChildWidget != PrevFocus)
	{
		if (PrevFocus)
		{
			PrevFocus->SetHasInputKeyFocus(false);
		}

		if (ChildWidget)
		{
			ChildThatHasInputKeyFocus = ChildWidget;
			ChildThatHasInputKeyFocus->SetHasInputKeyFocus(true);
		}
		else
		{
			ChildThatHasInputKeyFocus.Reset();
		}

		NativeOnChildThatHasInputKeyFocusChanged(PrevFocus, ChildWidget);
	}

	return true;
}

void UGUIWidget::Open()
{
	if (OpenCloseTween->GetValue() == false)
	{
		SetVisibility(OpenVisibility);
		OpenCloseTween->SetValue(true);
	}
}

void UGUIWidget::Close(FString Result)
{
	if (OpenCloseTween->GetValue() == true)
	{
		ResultUponClose = Result;
		SetVisibility(ESlateVisibility::HitTestInvisible);
		OpenCloseTween->SetValue(false);
	}
}

bool UGUIWidget::HandleInputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
	if (CanReceiveInputKey())
	{
		if (ChildThatHasInputKeyFocus == nullptr)
		{
			return NativeOnInputKey(EGUIInputKeyPhase::Target, Key, EventType, AmountDepressed, bGamepad);
		}
		else
		{
			if (NativeOnInputKey(EGUIInputKeyPhase::Capture, Key, EventType, AmountDepressed, bGamepad))
			{
				// Handled in capture phase of this widget
				return true;
			}

			if (ChildThatHasInputKeyFocus->HandleInputKey(Key, EventType, AmountDepressed, bGamepad))
			{
				// Handled by child
				return true;
			}

			// Do bubbling phase, return true if the bubbling input key event is handled by this widget
			return NativeOnInputKey(EGUIInputKeyPhase::Bubbling, Key, EventType, AmountDepressed, bGamepad);
		}
	}

	// Should only get here if this widget can not receive input key
	return false;
}


void UGUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if ( OpenCloseTween->GetValue() == false )
	{
		if ( bAutoOpenOnConstruct )
		{
			Open();
		}
		else
		{
			SetVisibility( CloseVisibility );
		}
	}
}

void UGUIWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UGUIWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if ( OpenCloseTween->IsTweening() )
	{
		if ( !OpenCloseTween->Tick(InDeltaTime) )
		{
			if ( OpenCloseTween->GetValue() == false )
			{
				SetVisibility(CloseVisibility);
			}
		}
	}

	Tweener->Tick( InDeltaTime );
	BooleanTweener->Tick( InDeltaTime );
	StateMachine->Tick( InDeltaTime );
}


void UGUIWidget::OnOpenCloseTweening_Implementation(float Alpha)
{
	FLinearColor Color;
	Color.R = Color.G = Color.B = 1.f;
	Color.A = FMath::Clamp(Alpha, 0.f, 1.f);
	SetColorAndOpacity(Color);
}

bool UGUIWidget::NativeOnInputKeyPressed(EGUIInputKeyPhase Phase, FKey Key, float AmountDepressed, bool bGamepad)
{
	return OnInputKeyPressed(Phase, Key, AmountDepressed, bGamepad).NativeReply.IsEventHandled();
}

bool UGUIWidget::NativeOnInputKeyReleased(EGUIInputKeyPhase Phase, FKey Key, float AmountDepressed, bool bGamepad)
{
	return OnInputKeyReleased(Phase, Key, AmountDepressed, bGamepad).NativeReply.IsEventHandled();
}

bool UGUIWidget::NativeOnInputKeyRepeat(EGUIInputKeyPhase Phase, FKey Key, float AmountDepressed, bool bGamepad)
{
	return OnInputKeyRepeat(Phase, Key, AmountDepressed, bGamepad).NativeReply.IsEventHandled();
}

void UGUIWidget::NativeOnAllowInputKeyChanged(bool bFlag)
{
	OnAllowInputKeyChanged(bFlag);
}

void UGUIWidget::NativeOnInputKeyFocusReceived()
{
	OnInputKeyFocusReceived();
}

void UGUIWidget::NativeOnInputKeyFocusLost()
{
	OnInputKeyFocusLost();
}

void UGUIWidget::NativeOnChildThatHasInputKeyFocusChanged(UGUIWidget* PreviousChild, UGUIWidget* NewChild)
{
	OnChildThatHasInputKeyFocusChanged(PreviousChild, NewChild);
}
