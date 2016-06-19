// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GameUIPrivatePCH.h"
#include "Blueprint/WidgetTree.h"
#include "GUIPlayerController.h"

AGUIPlayerController::AGUIPlayerController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bChangeMapWhenAllScreensAreClosed = false;
}

int32 AGUIPlayerController::GetScreenCount() const
{
	return Screens.Num();
}

UGUIScreen* AGUIPlayerController::GetScreenAt(int32 Index) const
{
	if (Index < 0 || Index >= Screens.Num())
	{
		return nullptr;
	}
	return Screens[Index];
}

UGUIScreen* AGUIPlayerController::GetTopMostScreen() const
{
	int32 Num = Screens.Num();
	return (Num == 0) ? nullptr : Screens[Num -1];
}

int32 AGUIPlayerController::GetScreenIndex(UGUIScreen* Screen) const
{
	for (int32 Index = 0; Index < Screens.Num(); Index++)
	{
		if (Screens[Index] == Screen)
		{
			return Index;
		}
	}
	return INDEX_NONE;
}

TArray<UGUIScreen*> AGUIPlayerController::FindAllScreensWithClass(TSubclassOf<UGUIScreen> ScreenClass) const
{
	TArray<UGUIScreen*> Result;
	for (UGUIScreen* Screen : Screens)
	{
		if (Screen->GetClass() == ScreenClass)
		{
			Result.Add(Screen);
		}
	}
	return Result;
}

UGUIScreen* AGUIPlayerController::FindFirstScreenWithClass(TSubclassOf<UGUIScreen> ScreenClass) const
{
	for (UGUIScreen* Screen : Screens)
	{
		if (Screen->IsA(ScreenClass))
		{
			return Screen;
		}
	}
	return nullptr;
}

UGUIScreen* AGUIPlayerController::CreateScreen(TSubclassOf<UGUIScreen> ScreenClass)
{
	return CreateWidget<UGUIScreen>(this, ScreenClass);
}

UGUIScreen* AGUIPlayerController::OpenScreen(UGUIScreen* Screen, ESuccess& Result)
{
	if (!Screen || Screen->IsPendingKill() || Screen->GetOwningPlayer() != this)
	{
		// The screen is not valid or we are not the owner of this screen.
		Result = ESuccess::Fail;
		return nullptr;
	}

	if (Screen->GetIsOpen())
	{
		// This Screen is already opened.
		Result = ESuccess::Success;
		return Screen;
	}

	// Set ZOrder to be one level higher that the current top most screen, we need to do this
	// before we close any screen, or else the closed screen animation will drawn on top of this new screen.
	UGUIScreen* TopMost = GetTopMostScreen();
	if (TopMost)
	{
		Screen->ZOrder = TopMost->ZOrder + 1;
	}
	else
	{
		Screen->ZOrder = 10;
	}

	if (Screen->GetIsRoot())
	{
		// This Screen is root, close all other Screens
		CloseAllScreens();
	}	
	else if (Screen->GetIsUnique())
	{
		// Close another Screen with the same class if this Screen is unique
		UGUIScreen* OtherScreen = FindFirstScreenWithClass(Screen->GetClass());
		if (OtherScreen)
		{
			CloseScreen(OtherScreen);
		}
	}

	// Deactivate the current top most screen
	TopMost = GetTopMostScreen();
	if (TopMost)
	{
		TopMost->SetIsActive(false);
	}

	// Open the Screen
	Screen->NativeOpen();
	Screen->SetIsActive(true);
	OnOpenScreen.Broadcast(Screen);
	Screens.Add(Screen);

	UpdateScreensImplications();

	Result = ESuccess::Success;

	return Screen;
}

void AGUIPlayerController::CloseScreen(UGUIScreen* Screen, FString Result)
{
	int32 ScreenIndex = GetScreenIndex(Screen);
	if (ScreenIndex == INDEX_NONE)
	{
		return;
	}

	Screens.RemoveAt(ScreenIndex);
	Screen->SetIsActive(false);
	Screen->NativeClose(Result);
	OnCloseScreen.Broadcast(Screen, Result);

	UpdateScreensImplications();
}

void AGUIPlayerController::CloseAllScreens(int32 FromIndex)
{
	TArray<UGUIScreen*> ScreensThatAreStillOpened;
	TArray<UGUIScreen*> ClosedScreens;

	for (int32 Index = 0; Index < Screens.Num(); Index++)
	{
		UGUIScreen* Screen = Screens[Index];
		if (Index < FromIndex)
		{
			ScreensThatAreStillOpened.Add(Screen);
		}
		else
		{
			ClosedScreens.Add(Screen);
			Screen->SetIsActive(false);
			Screen->NativeClose( TEXT("") );
			if ( bChangeMapWhenAllScreensAreClosed )
			{
				ClosingScreens.Add(Screen);
			}
		}
	}

	Screens = ScreensThatAreStillOpened;

	for (auto* Screen : ClosedScreens)
	{
		OnCloseScreen.Broadcast(Screen, TEXT(""));
	}
}


void AGUIPlayerController::CloseAllScreensAndChangeMap(FString MapName)
{
	bChangeMapWhenAllScreensAreClosed = true;
	ChangeMapName = MapName;
	CloseAllScreens();
}

UGUIScreen* AGUIPlayerController::OpenScreenWithClass(TSubclassOf<UGUIScreen> ScreenClass, ESuccess& OutResult)
{
	UGUIScreen* Screen = nullptr;

	OutResult = ESuccess::Fail;

	if ( !ScreenClass )
	{
		return nullptr;
	}

	UGUIScreen* CDO = ScreenClass->GetDefaultObject<UGUIScreen>();
	if (CDO->GetIsUnique())
	{
		Screen = FindFirstScreenWithClass(ScreenClass);
	}

	if (!Screen)
	{
		Screen = CreateScreen(ScreenClass);
	}
	
	if ( Screen )
	{
		OpenScreen( Screen, OutResult );
	}
	else
	{
		OutResult = ESuccess::Success;
	}
	
	return Screen;
}

void AGUIPlayerController::CloseScreenWithClass(TSubclassOf<UGUIScreen> ScreenClass)
{
	TArray<UGUIScreen*> Screens = FindAllScreensWithClass(ScreenClass);
	for (auto* Screen : Screens)
	{
		CloseScreen(Screen);
	}
}

UGUIDynamicCanvas* AGUIPlayerController::GetDynamicCanvas() const
{
	checkf(DynamicCanvas, TEXT("Trying to get an invalid dynamic canvas, have you already set DynamicCanvasClass in your player controller?"));
	return DynamicCanvas;
}

void AGUIPlayerController::BeginPlay()
{
	bChangeMapWhenAllScreensAreClosed = false;
	if (bAlwaysShowMouseCursor)
	{
		bShowMouseCursor = true;
	}

	if (DynamicCanvasClass != nullptr)
	{
		DynamicCanvas = CreateWidget<UGUIDynamicCanvas>(this, DynamicCanvasClass);
		DynamicCanvas->AddToViewport();
	}
	else
	{
		DynamicCanvas = CreateWidget<UGUIDynamicCanvas>(this, UGUIDynamicCanvas::StaticClass());
		UCanvasPanel* CanvasPanel = DynamicCanvas->WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
		DynamicCanvas->WidgetTree->RootWidget = CanvasPanel;
		DynamicCanvas->AddToViewport();
	}

	if ( Screens.Num() == 0 )
	{
		SetInputMode( FInputModeGameOnly() );
	}

	Super::BeginPlay();
}

void AGUIPlayerController::EndPlay(const EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);

	DynamicCanvas = nullptr;
}

bool AGUIPlayerController::InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
	UGUIScreen* TopMostScreen = GetTopMostScreen();
	if (TopMostScreen && TopMostScreen->CanReceiveInputKey())
	{
		if (TopMostScreen->HandleInputKey(Key, EventType, AmountDepressed, bGamepad) || TopMostScreen->GetIsModal())
		{
			return true;
		}
	}

	return Super::InputKey(Key, EventType, AmountDepressed, bGamepad);
}

void AGUIPlayerController::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);
	if ( bChangeMapWhenAllScreensAreClosed )
	{
		bool bChangeMap = true;
		for ( const TWeakObjectPtr<UGUIScreen>& Screen : ClosingScreens )
		{
			if ( Screen.IsValid() && Screen->GetOpenCloseTween()->GetAlpha() > 0 )
			{
				bChangeMap = false;
				break;
			}
		}
		if ( bChangeMap )
		{
			bChangeMapWhenAllScreensAreClosed = false;
			UGameplayStatics::OpenLevel( this, FName(*ChangeMapName) );
		}
	}
}

void AGUIPlayerController::UpdateScreensImplications()
{
	bool bShouldPauseTheGame = false;
	bool bModal = false;
	bool bIgnoreLookInput = false;
	bool bIgnoreMoveInput = false;
	bScreenStackRequiresMouseCursor = false;

	bool bAcceptMouse = true;

	for (int32 i = Screens.Num() - 1; i >= 0; i--)
	{
		UGUIScreen* Screen = Screens[i];
		if ( bAcceptMouse )		
		{
			Screen->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			Screen->SetVisibility(ESlateVisibility::HitTestInvisible);
		}

		if (Screen->GetShouldPauseTheGame())
		{
			bShouldPauseTheGame = true;
		}

		bScreenStackRequiresMouseCursor = Screen->GetShouldShowCursor();

		if (Screen->GetIsModal())
		{
			bModal = true;
			bAcceptMouse = false;
		}

		if (Screen->GetIgnoreMoveInput())
		{
			bIgnoreMoveInput = true;
		}

		if (Screen->GetIgnoreLookInput())
		{
			bIgnoreLookInput = true;
		}
	}

	if (bShouldPauseTheGame != IsPaused())
	{
		SetPause(bShouldPauseTheGame);
	}

	SetIgnoreLookInput(bModal || bIgnoreLookInput);
	SetIgnoreMoveInput(bModal || bIgnoreMoveInput);

	bShowMouseCursor = bAlwaysShowMouseCursor || bScreenStackRequiresMouseCursor;

	UGUIScreen* TopMostScreen = GetTopMostScreen();
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer)
	{
		TSharedPtr<SWidget> ScreenWidget = TopMostScreen->TakeWidget();
		FReply& SlateOperations = LocalPlayer->GetSlateOperations();
		SlateOperations.ReleaseMouseLock();
		if ( TopMostScreen && bShowMouseCursor )
		{
			SlateOperations.SetUserFocus( ScreenWidget.ToSharedRef() );
			SlateOperations.ReleaseMouseCapture();
			if ( TopMostScreen->bLockMouseToViewport )
			{
				UGameViewportClient* GameViewportClient = GetWorld()->GetGameViewport();					
				TSharedPtr<SViewport> ViewportWidget = GameViewportClient->GetGameViewportWidget();
				SlateOperations.LockMouseToWidget( ViewportWidget.ToSharedRef() );
			}
		}
	}
}

void AGUIPlayerController::SetGUIInputMethod(EGUIInputMethod InputMethod)
{
	if (GUIInputMethod != InputMethod)
	{
		GUIInputMethod = InputMethod;
		if (GUIInputMethod == EGUIInputMethod::Gamepad)
		{
			bShowMouseCursor = false;
		}
		else
		{
			bShowMouseCursor = bAlwaysShowMouseCursor || bScreenStackRequiresMouseCursor;
		}
		OnChangeGUIInputMethod.Broadcast(GUIInputMethod);
	}
}
