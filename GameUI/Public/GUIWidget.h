// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "GameUITypes.h"
#include "Blueprint/UserWidget.h"
#include "Util/Tween.h"
#include "GUIWidget.generated.h"

class AGUIPlayerController;

UCLASS(Blueprintable, BlueprintType)
class GAMEUI_API UGUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Constructor. */
	UGUIWidget(const FObjectInitializer& Initializer);

	/** Check if this widget is opened (or opening). */
	UFUNCTION(BlueprintPure, Category="GUI Widget")
	bool GetIsOpen() const;

	/** Open this widget. */
	UFUNCTION(BlueprintCallable, Category="GUI Widget")
	virtual void Open();

	/** Close this widget. */
	UFUNCTION(BlueprintCallable, Category="GUI Widget")
	virtual void Close(FString Result = TEXT(""));

	UFUNCTION(BlueprintPure, Category="GUIWidget")
	UTweeningBoolean* GetOpenCloseTween() const { return OpenCloseTween; }

	/** Get the result from the close operation of this widget. */
	UFUNCTION(BlueprintPure, Category="GUI Widget")
	FString GetResultUponClose() const;

	/** Get the GUIWidget parent of this widget. */
	UFUNCTION(BlueprintPure, Category="GUI Widget")
	UGUIWidget* GetGUIWidgetParent() const;

	/** Check whether this widget can receive input key.
	 * Widget that are not visible, not enabled, or specifically not allowed to process input are unable to receive input key.
	 * @see: SetAllowInputKey
	 */
	UFUNCTION(BlueprintPure, Category="Input Key")
	bool CanReceiveInputKey() const;

	/** Check Whether this widget allows input key. */
	UFUNCTION(BlueprintPure, Category="Input Key")
	bool GetAllowInputKey() const;

	/** Check whether this widget has input key focus. */
	UFUNCTION(BlueprintPure, Category="Input Key")
	virtual bool HasInputKeyFocus() const;

	/** Get the child of this widget that currently has the input key focus. */
	UFUNCTION(BlueprintPure, Category="Input Key")
	UGUIWidget* GetChildThatHasInputKeyFocus() const;

	/** Get the GUIPlayerController that owns this container. 
	 * This function is checked thus the application will breaks if this container doesn't belong to a valid GUIPlayerController.
	 * returns	GUIPlayerController that owns this widget.
	 */
	UFUNCTION(BlueprintPure, Category="Utility")
	AGUIPlayerController* GetGUIPlayerController() const;

	/** Set whether this widget allows input key. */
	UFUNCTION(BlueprintCallable, Category="Input Key")
	void SetAllowInputKey(bool bFlag);

	/** Set the child that has input key focus. 
	 * @param	ChildWidget		ChildWidget to get the input key focus. This parameter can be null, which means to not focus on any child.
	 * @return	true if the input key focus has successfully been switched to the ChildWidget
	 */
	UFUNCTION(BlueprintCallable, Category="Input Key")
	bool SetChildThatHasInputKeyFocus(UGUIWidget* ChildWidget);

	/** Request input key focus from this widget's parent.
	 * This is the same as calling GetGUIParent()->SetChildThatHasInputKeyFocus(this), but of course with a lot of safety codes.
	 * @return	true	if this widget able to receive focus from its parent.
	 */
	UFUNCTION(BlueprintCallable, Category="Input Key")
	virtual bool RequestInputKeyFocusFromParent();

	/** A native c++ function that set whether this widget has input key focus.
	 * Do not call this function unless you know what you're doing.
	 * @return false if this widget failed to acquire input key focus.
	 */
	virtual bool SetHasInputKeyFocus(bool bFlag);

	/** Handle incoming input key.
	 * Should only be called by widget's parent or GUIPlayerController.
	 * @return false if the input key is not handled by this widget.
	 */
	virtual bool HandleInputKey(FKey Key, EInputEvent Type, float AmountDepressed, bool bGamepad);

	// UUserWidget interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	// End of UUserWidget interface

protected:

	/** Whether this widget can receive input key focus. */
	UPROPERTY(EditAnywhere, Category="GUI Widget")
	bool bAllowInputKey = true;

	UPROPERTY(EditAnywhere, Category="GUI Widget")
	bool bAutoOpenOnConstruct = false;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Instanced, Category="GUI Widget")
	UTweeningBoolean* OpenCloseTween;

	/** This widget visibility when it's opened. */
	UPROPERTY(EditAnywhere, Category="GUI Widget")
	ESlateVisibility OpenVisibility = ESlateVisibility::SelfHitTestInvisible;

	/** This widget's visibility when it's closed. */
	UPROPERTY(EditAnywhere, Category="GUI Widget")
	ESlateVisibility CloseVisibility = ESlateVisibility::Hidden;

	/** A utility tweener for any purposes. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Instanced, Category="Tweener")
	UTweener* Tweener;

	/** A utility boolean tweener for any purposes. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Instanced, Category="Boolean Tweener")
	UBooleanTweener* BooleanTweener;

	/** A utility tweening state machine for any purposes. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Instanced, Category="State Machine")
	UTweeningStateMachine* StateMachine;

protected:

	/** Event that is triggered when this widget opens. */
	UFUNCTION(BlueprintImplementableEvent, Category="GUI Widget|Event")
	void OnOpenClose( bool bIsOpen );

	/** Event that is triggered every time the opening and closing animation is tweening.
	 * This should be used to change the appearance of the widget based on the Value.
	 * the range of the Value is from 0 to 1, where 0 should mean fully hidden and 1 should mean fully visible.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="GUI Widget")
	void OnOpenCloseTweening(float Alpha);

	/** Event that is triggered every time the opening and closing animation is finished.
	 * This event will usually does not really need to be handled, but it's there for some very special cases 
	 * (e.g trigger something when the widget is fully visible)
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="GUI Widget")
	void OnOpenCloseFinished( bool bIsOpen );

	/** Fall back native c++ function if OnOpenFlagTweenerTweening event is not handled in blueprint.
	 * This function will simply change the opacity of the widget based on Value.
	 */
	virtual void OnOpenCloseTweening_Implementation(float Alpha);	

	/** Event that is triggered when bAllowInputKey is changed. */
	UFUNCTION(BlueprintImplementableEvent, Category="Input Key")
	void OnAllowInputKeyChanged(bool bFlag);

	/** Event that is triggered when this widget received an input key focus. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category="Input Key")
	void OnInputKeyFocusReceived();

	/** Event that is triggered when this widget lost its input key focus. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category="Input Key")
	void OnInputKeyFocusLost();

	/** Event that is triggered when an input key is pressed. 
	 * @param	Phase			The phase of this input key, it can be capture, target, or bubbling
	 * @param	Key				The key for this event.
	 * @param	AmountDepressed	Only for analog key, how hard the input key is pressed
	 * @param	bGamepad		Just a utility param for Gamepad key detection
	 * @returns	Handled or not, if it's handled that the input key event flow will be stopped.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category="Input Key|Event")
	FEventReply OnInputKeyPressed(EGUIInputKeyPhase Phase, FKey Key, float AmountDepressed, bool bGamepad);

	/** Event that is triggered when an input key is released. 
	 * @param	Phase			The phase of this input key, it can be capture, target, or bubbling
	 * @param	Key				The key for this event.
	 * @param	AmountDepressed	Only for analog key, how hard the input key is pressed
	 * @param	bGamepad		Just a utility param for Gamepad key detection
	 * @returns	Handled or not, if it's handled that the input key event flow will be stopped.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category="Input Key|Event")
	FEventReply OnInputKeyReleased(EGUIInputKeyPhase Phase, FKey Key, float AmountDepressed, bool bGamepad);

	/** Event that is triggered when an input key is repeating.
	 * This is just like when you hold a keyboard key for a long time, you will get several characters showing in the text editor.
	 * This event will also be triggered for Gamepad keys.
	 * @param	Phase			The phase of this input key, it can be capture, target, or bubbling
	 * @param	Key				The key for this event.
	 * @param	AmountDepressed	Only for analog key, how hard the input key is pressed
	 * @param	bGamepad		Just a utility param for Gamepad key detection
	 * @returns	Handled or not, if it's handled that the input key event flow will be stopped.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category="Input Key|Event")
	FEventReply OnInputKeyRepeat(EGUIInputKeyPhase Phase, FKey Key, float AmountDepressed, bool bGamepad);

	/** Event that is triggered when the child of this widget that is has input focus is changed. */
	UFUNCTION(BlueprintImplementableEvent, Category="Input Key")
	void OnChildThatHasInputKeyFocusChanged(UGUIWidget* PreviousChild, UGUIWidget* NewChild);


	// Begin: C++ event handlers
	// Override these functions to do additional event handling in C++.
	// Do not forget to call its super class method so that the blueprint version of event handler is also called.
	virtual bool NativeOnInputKeyPressed(EGUIInputKeyPhase Phase, FKey Key, float AmountDepressed, bool bGamepad);
	virtual bool NativeOnInputKeyReleased(EGUIInputKeyPhase Phase, FKey Key, float AmountDepressed, bool bGamepad);
	virtual bool NativeOnInputKeyRepeat(EGUIInputKeyPhase Phase, FKey Key, float AmountDepressed, bool bGamepad);
	virtual void NativeOnAllowInputKeyChanged(bool bFlag);
	virtual void NativeOnInputKeyFocusReceived();
	virtual void NativeOnInputKeyFocusLost();
	virtual void NativeOnChildThatHasInputKeyFocusChanged(UGUIWidget* PreviousChild, UGUIWidget* NewChild);
	// End: C++ event handlers

private:

	/** Weak reference to child that has input key focus. */
	UPROPERTY(Transient)
	TWeakObjectPtr<UGUIWidget> ChildThatHasInputKeyFocus;

	/** Whether this widget has input key focus. */
	UPROPERTY(Transient)
	bool bHasInputKeyFocus = false;
	
	/** Store the result string that is provided from closing this widget. */
	UPROPERTY(Transient)
	FString ResultUponClose;

	/** Just a helper function to trigger the correct event depending on the input event type. */ 
	FORCEINLINE bool NativeOnInputKey(EGUIInputKeyPhase Phase, FKey Key, EInputEvent Type, float AmountDepressed, bool bGamepad)
	{
		switch(Type)
		{
		case EInputEvent::IE_Pressed:
			return NativeOnInputKeyPressed(Phase, Key, AmountDepressed, bGamepad);
		case EInputEvent::IE_Released:
			return NativeOnInputKeyReleased(Phase, Key, AmountDepressed, bGamepad);
		case EInputEvent::IE_Repeat:
			return NativeOnInputKeyRepeat(Phase, Key, AmountDepressed, bGamepad);
		default:
			return false;
		}
	}
};