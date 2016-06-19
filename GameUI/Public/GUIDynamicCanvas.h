// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
#pragma once
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "GUIWidget.h"
#include "GUIDynamicCanvas.generated.h"

USTRUCT(BlueprintType)
struct FAttachedWidgetData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttachedWidgetData")
	TWeakObjectPtr<UUserWidget> Widget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttachedWidgetData")
	TWeakObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttachedWidgetData")
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttachedWidgetData")
	FVector2D ScreenOffset;

	FORCEINLINE bool IsValid() const {
		return Widget.IsValid() && Widget->Slot && Widget->Slot->IsA(UCanvasPanelSlot::StaticClass()) && SceneComponent.IsValid();
	}

	FORCEINLINE UCanvasPanelSlot* GetSlot() const {
		return Cast<UCanvasPanelSlot>(Widget->Slot);
	}
};

UCLASS(Blueprintable)
class GAMEUI_API UGUIDynamicCanvas : public UGUIWidget
{
	GENERATED_BODY()
public:	
	UGUIDynamicCanvas(const FObjectInitializer& Initializer);

	UFUNCTION(BlueprintPure, Category="Dynamic Canvas")
	int32 GetWidgetCount() const;

	UFUNCTION(BlueprintPure, Category="Dynamic Canvas")
	UUserWidget* GetWidgetAt(int32 Index) const;

	UFUNCTION(BlueprintPure, Category="Dynamic Canvas")
	int32 GetWidgetIndex(UUserWidget* Widget) const;

	UFUNCTION(BlueprintPure, Category="Dynamic Canvas")
	UCanvasPanelSlot* GetWidgetSlotForWidget(UUserWidget* Widget) const;

	UFUNCTION(BlueprintPure, Category="Dynamic Canvas")
	TArray<UUserWidget*> FindWidgetsWithClass(TSubclassOf<UUserWidget> WidgetClass) const;

	UFUNCTION(BlueprintPure, Category="Dynamic Canvas")
	TArray<UUserWidget*> FindWidgetsAttachedToSceneComponent(USceneComponent* SceneComponent) const;

	UFUNCTION(BlueprintPure, Category="Dynamic Canvas")
	TArray<UUserWidget*> FindWidgetsAttachedToActor(AActor* Actor) const;

	UFUNCTION(BlueprintCallable, Category="Dynamic Canvas", meta=(AdvancedDisplay="1"))
	UCanvasPanelSlot* AddWidget(UUserWidget* Widget, bool bAutoSize = true, int32 ZOrder = 0);

	UFUNCTION(BlueprintCallable, Category="Dynamic Canvas", meta=(AdvancedDisplay="2"))
	UCanvasPanelSlot* AddWidgetAtPosition(UUserWidget* Widget, FVector2D Position, bool bAutoSize = true, int32 ZOrder = 0);

	UFUNCTION(BlueprintCallable, Category="Dynamic Canvas", meta=(AdvancedDisplay="4"))
	UCanvasPanelSlot* AttachWidgetToSceneComponent(UUserWidget* Widget, USceneComponent* SceneComponent, FName SocketName, FVector2D ScreenOffset, bool bAutoSize = true, int32 ZOrder = 0);

	UFUNCTION(BlueprintCallable, Category="Dynamic Canvas", meta=(AdvancedDisplay="4"))
	UCanvasPanelSlot* AttachWidgetToActor(UUserWidget* Widget, AActor* Actor, FName SocketName, FVector2D ScreenOffset, bool bAutoSize = true, int32 ZOrder = 0);

	UFUNCTION(BlueprintCallable, Category="Dynamic Canvas")
	bool RemoveWidget(UUserWidget* Widget);

	UFUNCTION(BlueprintCallable, Category="Dynamic Canvas")
	void RemoveWidgets(TArray<UUserWidget*> Widgets);

	UFUNCTION(BlueprintCallable, Category="Dynamic Canvas")
	void ClearWidgets();

	// UUserWidget interface
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;
	// End of UUserwidget interface
	
protected:

	UPROPERTY(Transient)
	TWeakObjectPtr<UCanvasPanel> WidgetCanvasPanel;

	UPROPERTY(Transient)
	TArray<FAttachedWidgetData> AttachedWidgetDataList;

	UPROPERTY(Transient)
	bool AttachedWidgetDataListIsDirty = false;

	UFUNCTION(BlueprintImplementableEvent, Category="Dynamic Canvas|Event")
	void OnWidgetAdded(UUserWidget* Widget);

	UFUNCTION(BlueprintImplementableEvent, Category="Dynamic Canvas|Event")
	void OnWidgetRemoved(UUserWidget* Widget);

	void UpdateAttachedWidget(const FAttachedWidgetData& Data);

	UCanvasPanel* FindCanvasPanel(UPanelWidget* Panel);
};