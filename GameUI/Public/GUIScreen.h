// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "GUIWidget.h"
#include "GUIScreen.generated.h"

UCLASS(Blueprintable)
class GAMEUI_API UGUIScreen : public UGUIWidget
{
	GENERATED_BODY()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOpenScreen);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCloseScreen, FString, Result);
public:

	friend class AGUIPlayerController;

	int32 ZOrder;

	UPROPERTY(BlueprintAssignable, Category = "GUI Screen")
	FOnOpenScreen OnOpenScreen;

	UPROPERTY(BlueprintAssignable, Category = "GUI Screen")
	FOnCloseScreen OnCloseScreen;

	UGUIScreen(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category="GUI Screen")
	bool GetIsActive() const;

	FORCEINLINE bool GetShouldPauseTheGame() const { return bPauseGame; }
	FORCEINLINE bool GetShouldShowCursor() const { return bShowCursor; }
	FORCEINLINE bool GetIsModal() const { return bIsModal; }
	FORCEINLINE bool GetIsRoot() const { return bIsRoot; }
	FORCEINLINE bool GetIsUnique() const { return bIsRoot || bIsUnique; }
	FORCEINLINE bool GetIgnoreMoveInput() const { return bIsModal || bIgnoreMoveInput; }
	FORCEINLINE bool GetIgnoreLookInput() const { return bIsModal || bIgnoreLookInput; }


	virtual void Open() override;
	virtual void Close(FString Result = FString()) override;
	virtual void NativeConstruct() override;

	void SetIsActive(bool bFlag);

protected:

	void NativeOpen();
	void NativeClose(FString Result);

	/** Should the game be paused when this screen is opened. */
	UPROPERTY(EditAnywhere, Category="GUI Screen")
	bool bPauseGame = false;

	/** Whether to consume input key even if the input key event is not handled. */
	UPROPERTY(EditAnywhere, Category="GUI Screen")
	bool bIsModal = false;

	UPROPERTY(EditAnywhere, Category="GUI Screen", meta=(EditCondition="!bIsModal"))
	bool bIgnoreMoveInput = false;

	UPROPERTY(EditAnywhere, Category="GUI Screen", meta=(EditCondition="!bIsModal"))
	bool bIgnoreLookInput = false;

	/** Should the game show cursor or not when this screen is opened. */
	UPROPERTY(EditAnywhere, Category="GUI Screen")
	bool bShowCursor = false;

	UPROPERTY(EditAnywhere, Category="GUIScreen", meta=(EditCondition="bShowCursor"))
	bool bLockMouseToViewport = false;

	/** Whether this is a root screen that always placed at the bottom of the screen stack. */
	UPROPERTY(EditAnywhere, Category="GUI Screen")
	bool bIsRoot = false;

	/** Whether there can be more than one screen with this class opened at the same time. Root screen is always unique */
	UPROPERTY(EditAnywhere, Category="GUI Screen", meta=(EditCondition="!bIsRoot"))
	bool bIsUnique = false;

	UPROPERTY()
	bool bIsActive = false;

	UFUNCTION(BlueprintImplementableEvent, Category="GUI Screen|Event")
	void OnActive();

	UFUNCTION(BlueprintImplementableEvent, Category="GUI Screen|Event")
	void OnInactive();

	UFUNCTION()
	void HandleOnOpenClose(bool bIsOpen );

	UFUNCTION()
	void HandleOnOpenCloseFinished(bool bIsOpen);

	// Begin Native event handlers, don't forget to call super method if you override these handlers.
	virtual void NativeOnActive();
	virtual void NativeOnInactive();
	// End Native event handlers

private:

	FString CloseResult;
};