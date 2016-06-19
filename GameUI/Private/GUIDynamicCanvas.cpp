// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#include "GameUIPrivatePCH.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "GUIDynamicCanvas.h"

UGUIDynamicCanvas::UGUIDynamicCanvas(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
	bAllowInputKey = false;
	OpenVisibility = ESlateVisibility::HitTestInvisible;
	CloseVisibility = ESlateVisibility::Hidden;
}

int32 UGUIDynamicCanvas::GetWidgetCount() const
{
	if (WidgetCanvasPanel.IsValid() == false)
	{
		return 0;
	}
	return WidgetCanvasPanel->GetChildrenCount();
}

UUserWidget* UGUIDynamicCanvas::GetWidgetAt(int32 Index) const
{
	if (WidgetCanvasPanel.IsValid() == false || Index < 0 || Index >= WidgetCanvasPanel->GetChildrenCount())
	{
		return nullptr;
	}

	return Cast<UUserWidget>(WidgetCanvasPanel->GetChildAt(Index));
}

int32 UGUIDynamicCanvas::GetWidgetIndex(UUserWidget* Widget) const
{
	if (WidgetCanvasPanel.IsValid() &&  Widget)
	{
		for (int32 Index = 0; Index < WidgetCanvasPanel->GetChildrenCount(); Index++)
		{
			if (WidgetCanvasPanel->GetChildAt(Index) == Widget)
			{
				return Index;
			}
		}
	}

	return INDEX_NONE;
}

UCanvasPanelSlot* UGUIDynamicCanvas::GetWidgetSlotForWidget(UUserWidget* Widget) const
{
	return Cast<UCanvasPanelSlot>(Widget->Slot);
}

TArray<UUserWidget*> UGUIDynamicCanvas::FindWidgetsWithClass(TSubclassOf<UUserWidget> WidgetClass) const
{
	TArray<UUserWidget*> Result;

	if (WidgetCanvasPanel.IsValid())
	{
		for (int32 Index = 0; Index < WidgetCanvasPanel->GetChildrenCount(); Index++)
		{
			UUserWidget* Widget = Cast<UUserWidget>(WidgetCanvasPanel->GetChildAt(Index));
			if (Widget && Widget->IsA(WidgetClass))
			{
				Result.Add(Widget);
			}
		}
	}
	
	return Result;
}

TArray<UUserWidget*> UGUIDynamicCanvas::FindWidgetsAttachedToSceneComponent(USceneComponent* SceneComponent) const
{
	TArray<UUserWidget*> Result;

	for (int32 Index = 0; Index < AttachedWidgetDataList.Num(); Index++)
	{
		const FAttachedWidgetData& Data = AttachedWidgetDataList[Index];
		if (Data.IsValid() && Data.SceneComponent.Get() == SceneComponent)
		{
			UUserWidget* Widget = Data.Widget.Get();
			Result.Add(Widget);
		}
	}

	return Result;
}

TArray<UUserWidget*> UGUIDynamicCanvas::FindWidgetsAttachedToActor(AActor* Actor) const
{
	TArray<UUserWidget*> Result;

	for (int32 Index = 0; Index < AttachedWidgetDataList.Num(); Index++)
	{
		const FAttachedWidgetData& Data = AttachedWidgetDataList[Index];
		if (Data.IsValid() && Data.SceneComponent.IsValid() && Data.SceneComponent->GetOwner() == Actor)
		{
			UUserWidget* Widget = Data.Widget.Get();
			Result.Add(Widget);
		}
	}

	return Result;
}

UCanvasPanelSlot* UGUIDynamicCanvas::AddWidget(UUserWidget* Widget, bool bAutoSize, int32 ZOrder)
{
	if (Widget && WidgetCanvasPanel.IsValid() && WidgetCanvasPanel->HasChild(Widget) == false)
	{
		UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(WidgetCanvasPanel->AddChild(Widget));
		Slot->SetAutoSize(bAutoSize);
		Slot->SetZOrder(ZOrder);
		OnWidgetAdded(Widget);
		return Slot;
	}

	return nullptr;
}

UCanvasPanelSlot* UGUIDynamicCanvas::AddWidgetAtPosition(UUserWidget* Widget, FVector2D Position, bool bAutoSize, int32 ZOrder)
{
	UCanvasPanelSlot* Slot = AddWidget(Widget, bAutoSize, ZOrder);
	if (Slot)
	{
		Slot->SetPosition(Position);
	}
	return Slot;
}

UCanvasPanelSlot* UGUIDynamicCanvas::AttachWidgetToSceneComponent(UUserWidget* Widget, USceneComponent* SceneComponent, FName SocketName, FVector2D ScreenOffset, bool bAutoSize, int32 ZOrder)
{
	if (!SceneComponent)
	{
		return nullptr;
	}

	UCanvasPanelSlot* Slot = AddWidget(Widget, bAutoSize, ZOrder);
	if (Slot)
	{
		FAttachedWidgetData Data;
		Data.Widget = Widget;
		Data.SceneComponent = SceneComponent;
		Data.SocketName = SocketName;
		Data.ScreenOffset = ScreenOffset;
		AttachedWidgetDataList.Add(Data);
		UpdateAttachedWidget(Data);
	}

	return Slot;
}

UCanvasPanelSlot* UGUIDynamicCanvas::AttachWidgetToActor(UUserWidget* Widget, AActor* Actor, FName SocketName, FVector2D ScreenOffset, bool bAutoSize, int32 ZOrder)
{
	if (!Actor || !Actor->GetRootComponent())
	{
		return nullptr;
	}

	return AttachWidgetToSceneComponent(Widget, Actor->GetRootComponent(), SocketName, ScreenOffset, bAutoSize, ZOrder);
}

bool UGUIDynamicCanvas::RemoveWidget(UUserWidget* Widget)
{
	bool bResult = false;
	if (WidgetCanvasPanel.IsValid())
	{
		bResult = WidgetCanvasPanel->RemoveChild(Widget);
		if (bResult)
		{
			OnWidgetRemoved(Widget);
		}
	}

	return bResult;
}

void UGUIDynamicCanvas::RemoveWidgets(TArray<UUserWidget*> Widgets)
{
	if (WidgetCanvasPanel.IsValid())
	{
		for (UUserWidget* Widget : Widgets)
		{
			if (WidgetCanvasPanel->RemoveChild(Widget))
			{
				OnWidgetRemoved(Widget);
			}
		}
	}
}

void UGUIDynamicCanvas::ClearWidgets()
{
	if (WidgetCanvasPanel.IsValid())
	{
		TArray<UUserWidget*> Widgets;
		for (int32 Index = 0; Index < WidgetCanvasPanel->GetChildrenCount(); Index++)
		{
			UUserWidget* Widget = Cast<UUserWidget>(WidgetCanvasPanel->GetChildAt(Index));
			if (Widget)
			{
				Widgets.Add(Widget);
			}
		}

		WidgetCanvasPanel->ClearChildren();

		for (UUserWidget* Widget : Widgets)
		{
			OnWidgetRemoved(Widget);
		}
	}
}

void UGUIDynamicCanvas::NativeConstruct()
{
	WidgetCanvasPanel.Reset();

	UPanelWidget* RootPanelWidget = Cast<UPanelWidget>(GetRootWidget());
	if (RootPanelWidget)
	{
		WidgetCanvasPanel = FindCanvasPanel(RootPanelWidget);
	}

	Super::NativeConstruct();
}

void UGUIDynamicCanvas::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (AttachedWidgetDataList.Num() > 0)
	{
		for (const auto& Data : AttachedWidgetDataList)
		{
			UpdateAttachedWidget(Data);
		}

		if (AttachedWidgetDataListIsDirty)
		{
			TArray<FAttachedWidgetData> CleanedList;
			for (int32 Index = 0; Index < AttachedWidgetDataList.Num(); Index++)
			{
				FAttachedWidgetData& Data = AttachedWidgetDataList[Index];
				if (Data.IsValid())
				{
					CleanedList.Add(Data);
				}
				else
				{
					if (Data.Widget.IsValid() && WidgetCanvasPanel.IsValid())
					{
						RemoveWidget(Data.Widget.Get());
					}
				}
			}
			AttachedWidgetDataList = CleanedList;
			AttachedWidgetDataListIsDirty = false;
		}
	}
}

void UGUIDynamicCanvas::NativeDestruct()
{
	WidgetCanvasPanel.Reset();
	Super::NativeDestruct();
}

void UGUIDynamicCanvas::UpdateAttachedWidget(const FAttachedWidgetData& Data)
{
	if (Data.IsValid() == false)
	{
		AttachedWidgetDataListIsDirty = true;
		return;
	}

	FVector WorldLocation = Data.SceneComponent->GetSocketLocation(Data.SocketName);
	FVector2D ScreenLocation;
	UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), WorldLocation, ScreenLocation);
	FVector2D Location = ScreenLocation / UWidgetLayoutLibrary::GetViewportScale(this);	
	Location = Location + Data.ScreenOffset;	

	UCanvasPanelSlot* Slot = Data.GetSlot();
	Slot->SetPosition(Location);
}

UCanvasPanel* UGUIDynamicCanvas::FindCanvasPanel(UPanelWidget* Panel)
{
	UCanvasPanel* CanvasPanel = Cast<UCanvasPanel>(Panel);
	for (int32 Index = 0; !CanvasPanel && (Index < Panel->GetChildrenCount()); Index++)
	{
		UWidget* ChildWidget = Panel->GetChildAt(Index);
		CanvasPanel = Cast<UCanvasPanel>(ChildWidget);
		if (!CanvasPanel)
		{
			UPanelWidget* ChildPanel = Cast<UPanelWidget>(ChildWidget);
			if (ChildPanel)
			{
				CanvasPanel = FindCanvasPanel(ChildPanel);
			}
		}
	}
	return CanvasPanel;
}