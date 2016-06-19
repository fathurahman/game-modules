// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once

#include "GameUITypes.h"
#include "GUIScreen.h"
#include "GUIDynamicCanvas.h"
#include "GUIPlayerController.generated.h"

//
//
//
UCLASS(meta=(ShortTooltip="This should be the base class of any controller that is used in GameMode which utilitizes GameUI features"))
class GAMEUI_API AGUIPlayerController: public APlayerController
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeGUIInputMethod, EGUIInputMethod, InputMethod);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOpenScreen, UGUIScreen*, Screen);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCloseScreen, UGUIScreen*, Screen, FString, Result);

	friend class UGUIScreen;

public:


	/** Constructor. */
	AGUIPlayerController(const FObjectInitializer& ObjectInitializer);

	//
	// Screen operation
	//

	/** Get the number of opened Screens. */	 
	UFUNCTION(BlueprintPure, Category="Screen")
	int32 GetScreenCount() const;

	UFUNCTION(BlueprintPure, Category="Screen")
	UGUIScreen* GetScreenAt(int32 Index) const;	

	UFUNCTION(BlueprintPure, Category="Screen")
	UGUIScreen* GetTopMostScreen() const;

	UFUNCTION(BlueprintPure, Category="Screen")
	int32 GetScreenIndex(UGUIScreen* Screen) const;

	UFUNCTION(BlueprintPure, Category="Screen")
	TArray<UGUIScreen*> FindAllScreensWithClass(TSubclassOf<UGUIScreen> ScreenClass) const;

	UFUNCTION(BlueprintPure, Category="Screen")
	UGUIScreen* FindFirstScreenWithClass(TSubclassOf<UGUIScreen> ScreenClass) const;

	UFUNCTION(BlueprintCallable, Category="Screen")
	UGUIScreen* CreateScreen(TSubclassOf<UGUIScreen> ScreenClass);

	UFUNCTION(BlueprintCallable, Category="Screen", meta=(ExpandEnumAsExecs="OutResult"))
	UGUIScreen* OpenScreen(UGUIScreen* Screen, ESuccess& OutResult);

	UFUNCTION(BlueprintCallable, Category="Screen")
	void CloseScreen(UGUIScreen* Screen, FString Result = TEXT(""));

	UFUNCTION(BlueprintCallable, Category="Screen")
	void CloseAllScreens(int32 FromIndex = 0);

	UFUNCTION(BlueprintCallable, Category="GUIPlayerController")
	void CloseAllScreensAndChangeMap( FString MapName );

	UFUNCTION(BlueprintCallable, Category="Screen", meta=(ExpandEnumAsExecs="OutResult"))
	UGUIScreen* OpenScreenWithClass(TSubclassOf<UGUIScreen> ScreenClass, ESuccess& OutResult);

	UFUNCTION(BlueprintCallable, Category="Screen")
	void CloseScreenWithClass(TSubclassOf<UGUIScreen> ScreenClass);

	UFUNCTION(BlueprintCallable, Category="Dynamic Canvas")
	UGUIDynamicCanvas* GetDynamicCanvas() const;

	/** Event triggered when input method changed. */
	UPROPERTY(BlueprintAssignable, Category="GUI|Event")
	FOnChangeGUIInputMethod OnChangeGUIInputMethod;

	/** Even triggered when a screen opens. */
	UPROPERTY(BlueprintAssignable, Category="GUI|Event")
	FOnOpenScreen OnOpenScreen;

	/** Event triggered when a sceen closes. */
	UPROPERTY(BlueprintAssignable, Category="GUI|Event")
	FOnCloseScreen OnCloseScreen;


	//////////////////////////////////////////////////////////////////////////
	// Input
	//////////////////////////////////////////////////////////////////////////

	// Begin Overrides.
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	virtual bool InputKey(FKey Key,EInputEvent EventType,float AmountDepressed,bool bGamepad) override;
	virtual void TickActor(float DeltaTime,enum ELevelTick TickType,FActorTickFunction& ThisTickFunction) override;
	// End Overrides.

protected:

	UPROPERTY(EditDefaultsOnly, Category="GUI Player Controller")
	bool bAlwaysShowMouseCursor = false;

	UPROPERTY(EditDefaultsOnly, Category="GUI Player Controller")
	TSubclassOf<UGUIDynamicCanvas> DynamicCanvasClass;

private:

	// TODO: move this to game instance
	UPROPERTY(Transient)
	EGUIInputMethod GUIInputMethod;

	UPROPERTY(Transient)
	TArray<UGUIScreen*> Screens;

	UPROPERTY(Transient)
	bool bScreenStackRequiresMouseCursor;

	UPROPERTY(Transient)
	UGUIDynamicCanvas* DynamicCanvas;

	void UpdateScreensImplications();

	void SetGUIInputMethod(EGUIInputMethod InputMethod);

	bool bChangeMapWhenAllScreensAreClosed;
	FString ChangeMapName;
	TArray<TWeakObjectPtr<UGUIScreen>> ClosingScreens;
};

