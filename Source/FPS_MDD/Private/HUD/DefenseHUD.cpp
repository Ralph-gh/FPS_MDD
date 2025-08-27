// DefenseHUD.cpp
#include "HUD/DefenseHUD.h"
#include "GUI/Slate/SSettingsWidget.h"
#include "Widgets/SWeakWidget.h"

#include "Engine/Canvas.h"
#include "CanvasItem.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include <Kismet/GameplayStatics.h>

void ADefenseHUD::BeginPlay()
{
	Super::BeginPlay();

    const FString Map = UGameplayStatics::GetCurrentLevelName(this,true);
    if (Map.Equals(TEXT("Endgame"), ESearchCase::IgnoreCase))
    {
        bShowGameEnded = true;   // <-- new flag
    }
    else
    {
        ShowSettingsMenu();
    }
}

void ADefenseHUD::SetBaseHealth(float InCurrent, float InMax)
{
	MaxHealth = FMath::Max(1.f, InMax);
	CurrentHealth = FMath::Clamp(InCurrent, 0.f, MaxHealth);
}

void ADefenseHUD::ShowSettingsMenu()
{
    if (bSettingsVisible || !GEngine || !GEngine->GameViewport) return;

    // Build the widget and add to viewport
    SettingsWidget = SNew(SSettingsWidget).OwnerHUD(TWeakObjectPtr<ADefenseHUD>(this));
    SlateWidgetContainer = SNew(SWeakWidget).PossiblyNullContent(SettingsWidget.ToSharedRef());
    GEngine->GameViewport->AddViewportWidgetContent(SlateWidgetContainer.ToSharedRef());

    if (PlayerOwner)
    {
        PlayerOwner->bShowMouseCursor = true;
        PlayerOwner->SetInputMode(FInputModeUIOnly());
        PlayerOwner->SetPause(true);
    }

    bSettingsVisible = true;
}

void ADefenseHUD::HideSettingsMenu()
{
    if (!bSettingsVisible || !GEngine || !GEngine->GameViewport) return;

    // Prefer to remove the container we added
    bool bRemoved = false;
    if (SlateWidgetContainer.IsValid())
    {
        GEngine->GameViewport->RemoveViewportWidgetContent(SlateWidgetContainer.ToSharedRef());
        bRemoved = true;
    }
    else if (SettingsWidget.IsValid())
    {
        // If for any reason the container is invalid, try removing the inner widget itself.
        GEngine->GameViewport->RemoveViewportWidgetContent(SettingsWidget.ToSharedRef());
        bRemoved = true;
    }

    // As a safety net (in case it was added more than once), collapse visibility too.
    if (SettingsWidget.IsValid())
    {
        SettingsWidget->SetVisibility(EVisibility::Collapsed);
    }

    SlateWidgetContainer.Reset();
    SettingsWidget.Reset();

    if (PlayerOwner)
    {
        PlayerOwner->bShowMouseCursor = false;
        PlayerOwner->SetInputMode(FInputModeGameOnly());
        PlayerOwner->SetPause(false);
    }

    bSettingsVisible = false;

    UE_LOG(LogTemp, Display, TEXT("HideSettingsMenu: removed=%d"), bRemoved ? 1 : 0);
}

void ADefenseHUD::DrawHUD()
{
	Super::DrawHUD();

	UE_LOG(LogTemp, Verbose, TEXT("ADefenseHUD::DrawHUD() called"));

	if (!Canvas) return;

    if (bShowGameEnded)
    {
        const FString Msg = TEXT("GAME ENDED");

        // Use an engine font (UFont*)
        UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;
        FCanvasTextItem Item(FVector2D::ZeroVector, FText::FromString(TEXT("GAME ENDED")), Font, FLinearColor::Red);
        Item.EnableShadow(FLinearColor::Black);
        Item.Scale = FVector2D(7.5f * (Canvas->ClipY / 1080.f)); // same scale both axes
        Item.bCentreX = true;
        Item.bCentreY = true;
        Canvas->DrawItem(Item, FVector2D(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f));

        return; // skip other HUD when showing end screen
    }


	const float Ratio = FMath::Clamp(CurrentHealth / FMath::Max(1.f, MaxHealth), 0.f, 1.f);

	// Background
	DrawRectFilled(FLinearColor(0.f, 0.f, 0.f, 0.6f), BarPos, BarSize);

	// Fill
	DrawRectFilled(FLinearColor(0.1f, 0.8f, 0.2f, 0.9f),
		BarPos, FVector2D(BarSize.X * Ratio, BarSize.Y));

	// Label
	if (GEngine)
	{
		Canvas->DrawText(
			GEngine->GetLargeFont(),
			FString::Printf(TEXT("Base %d / %d"),
				FMath::RoundToInt(CurrentHealth),
				FMath::RoundToInt(MaxHealth)),
			BarPos.X, BarPos.Y - 32.f
		);
	}
}

void ADefenseHUD::DrawRectFilled(const FLinearColor& Color, const FVector2D& Pos, const FVector2D& Size)
{
	FCanvasTileItem Tile(Pos, Size, Color);
	Tile.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Tile);
}

void ADefenseHUD::ShowEndLevelButton()
{
    if (bEndButtonVisible || !GEngine || !GEngine->GameViewport) return;

    EndLevelWidget =
        SNew(SOverlay)

        // top-right aligned slot
        + SOverlay::Slot()
        .HAlign(HAlign_Right)
        .VAlign(VAlign_Top)
        .Padding(FMargin(12.f))
        [
            SNew(SBorder)
                .Padding(FMargin(8.f, 4.f))
                .BorderBackgroundColor(FLinearColor(0, 0, 0, 0.6f))
                [
                    SNew(SButton)
                        .OnClicked(FOnClicked::CreateUObject(this, &ADefenseHUD::OnEndLevelClicked))
                        [
                            SNew(STextBlock).Text(FText::FromString(TEXT("EndLevel")))
                        ]
                ]
        ];

    // Add exactly this widget (no wrappers)
    GEngine->GameViewport->AddViewportWidgetContent(EndLevelWidget.ToSharedRef(), 900);
    bEndButtonVisible = true;
}

void ADefenseHUD::HideEndLevelButton()
{
    if (!bEndButtonVisible || !GEngine || !GEngine->GameViewport) return;

    if (EndLevelWidget.IsValid())
    {
        GEngine->GameViewport->RemoveViewportWidgetContent(EndLevelWidget.ToSharedRef());
    }
    EndLevelWidget.Reset();
    bEndButtonVisible = false;
}

static void LogCurrentMap(UObject* WorldContext, const TCHAR* Tag)
{
    const FString Name = UGameplayStatics::GetCurrentLevelName(WorldContext, /*bRemovePrefix=*/true);
    UE_LOG(LogTemp, Display, TEXT("[%s] CurrentLevel=%s"), Tag, *Name);
}

FReply ADefenseHUD::OnEndLevelClicked()
{
    
    LogCurrentMap(this, TEXT("BeforeOpenLevel"));

    // Load your Endgame map (ensure a level named "Endgame" exists)
    HideEndLevelButton();

    if (PlayerOwner)
    {
        PlayerOwner->bShowMouseCursor = false;
        PlayerOwner->SetInputMode(FInputModeGameOnly());
    }

    UGameplayStatics::OpenLevel(this, FName(TEXT("/Game/Maps/Endgame")));
    LogCurrentMap(this, TEXT("AfterOpenLevel"));
    return FReply::Handled();
}

void ADefenseHUD::ToggleEndLevelButton()
{
    if (IsEndLevelButtonVisible()) HideEndLevelButton();
    else                           ShowEndLevelButton();
}

