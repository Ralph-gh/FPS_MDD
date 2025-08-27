// DefenseHUD.cpp
#include "HUD/DefenseHUD.h"
#include "GUI/Slate/SSettingsWidget.h"
#include "GUI/Slate/SSMainMenuWidget.h" // <-- include your main menu Slate
#include "Widgets/SWeakWidget.h"
#include "Engine/Canvas.h"
#include "CanvasItem.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void ADefenseHUD::BeginPlay()
{
	Super::BeginPlay();

	const FString Map = UGameplayStatics::GetCurrentLevelName(this, /*bRemovePrefix=*/true);

	// Dedicated menu level
	if (Map.Equals(TEXT("MainMenu"), ESearchCase::IgnoreCase))
	{
		ShowMainMenu();
		return;
	}

	// Dedicated end screen level (DrawHUD will paint big red text)
	if (Map.Equals(TEXT("Endgame"), ESearchCase::IgnoreCase))
	{
		bShowGameEnded = true; // DrawHUD will early-return after painting end text
		return;
	}

	// Gameplay levels: do NOT auto-show settings. Call ShowSettingsMenu() from input instead.
}

void ADefenseHUD::SetBaseHealth(float InCurrent, float InMax)
{
	MaxHealth = FMath::Max(1.f, InMax);
	CurrentHealth = FMath::Clamp(InCurrent, 0.f, MaxHealth);
}

void ADefenseHUD::ShowSettingsMenu()
{
	if (bSettingsVisible || !GEngine || !GEngine->GameViewport) return;

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

	if (SlateWidgetContainer.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(SlateWidgetContainer.ToSharedRef());
	}
	else if (SettingsWidget.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(SettingsWidget.ToSharedRef());
	}

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
}

void ADefenseHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas) return;

	// Endgame big text overlay
	if (bShowGameEnded)
	{
		UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;
		FCanvasTextItem Item(FVector2D::ZeroVector, FText::FromString(TEXT("GAME ENDED")), Font, FLinearColor::Red);
		Item.EnableShadow(FLinearColor::Black);
		Item.Scale = FVector2D(7.5f * (Canvas->ClipY / 1080.f));
		Item.bCentreX = true;
		Item.bCentreY = true;
		Canvas->DrawItem(Item, FVector2D(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f));
		return; // Skip other HUD when showing end screen
	}

	// Health bar
	const float Ratio = FMath::Clamp(CurrentHealth / FMath::Max(1.f, MaxHealth), 0.f, 1.f);
	DrawRectFilled(FLinearColor(0.f, 0.f, 0.f, 0.6f), BarPos, BarSize);
	DrawRectFilled(FLinearColor(0.1f, 0.8f, 0.2f, 0.9f), BarPos, FVector2D(BarSize.X * Ratio, BarSize.Y));

	if (GEngine)
	{
		Canvas->DrawText(GEngine->GetLargeFont(),
			FString::Printf(TEXT("Base %d / %d"),
				FMath::RoundToInt(CurrentHealth),
				FMath::RoundToInt(MaxHealth)),
			BarPos.X, BarPos.Y - 32.f);
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

void ADefenseHUD::ShowMainMenu()
{
	if (bMainMenuVisible || !GEngine || !GEngine->GameViewport) return;

	// 1) Build the menu widget
	MainMenuWidget = SNew(SMainMenuWidget).OwnerHUD(TWeakObjectPtr<ADefenseHUD>(this));

	// 2) Upcast to SWidget and wrap in SWeakWidget
	TSharedRef<SWidget> MenuAsWidget = StaticCastSharedRef<SWidget>(MainMenuWidget.ToSharedRef());
	MainMenuContainer = SNew(SWeakWidget).PossiblyNullContent(MenuAsWidget);

	// 3) Add to viewport
	GEngine->GameViewport->AddViewportWidgetContent(MainMenuContainer.ToSharedRef());

	// 4) UI-only input & pause
	if (PlayerOwner)
	{
		PlayerOwner->bShowMouseCursor = true;
		PlayerOwner->SetInputMode(FInputModeUIOnly());
		PlayerOwner->SetPause(true);
	}

	bMainMenuVisible = true;
}

void ADefenseHUD::HideMainMenu()
{
	if (!bMainMenuVisible || !GEngine || !GEngine->GameViewport) return;

	if (MainMenuContainer.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(MainMenuContainer.ToSharedRef());
	}
	MainMenuContainer.Reset();
	MainMenuWidget.Reset();

	if (PlayerOwner)
	{
		PlayerOwner->bShowMouseCursor = false;
		PlayerOwner->SetInputMode(FInputModeGameOnly());
		PlayerOwner->SetPause(false);
	}

	bMainMenuVisible = false;
}
