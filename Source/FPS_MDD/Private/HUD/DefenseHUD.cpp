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
#include "GUI/Slate/SEndgameMenuWidget.h"

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
		ShowEndgameMenu();               // <-- show the Play/Quit menu on game end too
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

	const float CX = Canvas->ClipX * 0.5f;
	const float CY = Canvas->ClipY * 0.5f;

	if (CrosshairTexture)
	{
		// --- Texture version ---
		const float TexW = CrosshairTexture->GetSizeX() * CrosshairTexScale;
		const float TexH = CrosshairTexture->GetSizeY() * CrosshairTexScale;

		const FVector2D DrawPos(CX - TexW * 0.5f, CY - TexH * 0.5f);
		FCanvasTileItem Tile(DrawPos, CrosshairTexture->GetResource(), FVector2D(TexW, TexH), CrosshairColor);
		Tile.BlendMode = SE_BLEND_Translucent; // so PNG alpha works
		Canvas->DrawItem(Tile);
	}
	else
	{
		// --- Line-drawn version (no texture required) ---
		const float Gap = CrosshairGap;
		const float Arm = CrosshairArm;
		const float T = CrosshairThickness;

		// Horizontal
		Canvas->K2_DrawLine(FVector2D(CX - Gap - Arm, CY), FVector2D(CX - Gap, CY), T, CrosshairColor);
		Canvas->K2_DrawLine(FVector2D(CX + Gap, CY), FVector2D(CX + Gap + Arm, CY), T, CrosshairColor);

		// Vertical
		Canvas->K2_DrawLine(FVector2D(CX, CY - Gap - Arm), FVector2D(CX, CY - Gap), T, CrosshairColor);
		Canvas->K2_DrawLine(FVector2D(CX, CY + Gap), FVector2D(CX, CY + Gap + Arm), T, CrosshairColor);
	}

	if (!Canvas) return;

	// Endgame big text overlay
	if (bShowGameEnded)
	{
		UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;
		FCanvasTextItem Item(FVector2D::ZeroVector, FText::FromString(TEXT("Game Over")), Font, FLinearColor::Red);
		Item.EnableShadow(FLinearColor::Black);
		Item.Scale = FVector2D(7.5f * (Canvas->ClipY / 1080.f));
		Item.bCentreX = true;
		Item.bCentreY = true;
		Canvas->DrawItem(Item, FVector2D(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.9f));
		return; // Skip other HUD when showing end screen
	}

	// Health bar
	const float Ratio = FMath::Clamp(CurrentHealth / FMath::Max(1.f, MaxHealth), 0.f, 1.f);
	DrawRectFilled(FLinearColor(0.f, 0.f, 0.f, 0.6f), BarPos, BarSize);
	DrawRectFilled(FLinearColor(0.1f, 0.8f, 0.2f, 0.9f), BarPos, FVector2D(BarSize.X * Ratio, BarSize.Y));

	UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;
	const float Dpi = Canvas->ClipY / 1080.f;

	// ---- Big "Base xx / yy" above the bar ----
	{
		const FString BaseText = FString::Printf(
			TEXT("Base %d / %d"),
			FMath::RoundToInt(CurrentHealth),FMath::RoundToInt(MaxHealth));

		FCanvasTextItem BaseItem(FVector2D::ZeroVector, FText::FromString(BaseText), Font, FLinearColor::White);
		BaseItem.EnableShadow(FLinearColor::Black);
		BaseItem.Scale = FVector2D(2.0f * Dpi, 2.0f * Dpi);   // bigger than default
		const FVector2D BasePos(BarPos.X, BarPos.Y + 60.f);   // a bit higher than the bar
		Canvas->DrawItem(BaseItem, BasePos);

		// --- Score label (explicit style) ---
		const FString ScoreText = FString::Printf(TEXT("Score: %d"), CurrentScore);
		FCanvasTextItem ScoreItem(FVector2D::ZeroVector, FText::FromString(ScoreText), Font, FLinearColor::White);
		ScoreItem.EnableShadow(FLinearColor::Black);
		ScoreItem.Scale = FVector2D(1.6f * Dpi, 1.6f * Dpi);
		const FVector2D ScorePos(BarPos.X, BarPos.Y + BarSize.Y + 18.f);
		Canvas->DrawItem(ScoreItem, ScorePos);
	}

	// ---- "Wave N" under the bar ----
	{
		const FString WaveText = FString::Printf(TEXT("Wave %d"), CurrentWave);

		FCanvasTextItem WaveItem(FVector2D::ZeroVector, FText::FromString(WaveText), Font, FLinearColor::Yellow);
		WaveItem.EnableShadow(FLinearColor::Black);
		WaveItem.Scale = FVector2D(1.4f * Dpi, 1.4f * Dpi);
		const FVector2D WavePos(BarPos.X, BarPos.Y + BarSize.Y + 0.0f); // just below the bar
		Canvas->DrawItem(WaveItem, WavePos);
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

void ADefenseHUD::ToggleSettingsMenu()
{
	if (bSettingsVisible)  HideSettingsMenu();
	else                   ShowSettingsMenu();
}
void ADefenseHUD::ShowEndgameMenu()
{
	if (bEndgameMenuVisible || !GEngine || !GEngine->GameViewport) return;

	// If settings/main menu still up, hide them
	if (bSettingsVisible) HideSettingsMenu();

	EndgameMenuWidget = SNew(SEndgameMenuWidget).OwnerHUD(TWeakObjectPtr<ADefenseHUD>(this));

	TSharedRef<SWidget> AsWidget = StaticCastSharedRef<SWidget>(EndgameMenuWidget.ToSharedRef());
	EndgameMenuContainer = SNew(SWeakWidget).PossiblyNullContent(AsWidget);

	GEngine->GameViewport->AddViewportWidgetContent(EndgameMenuContainer.ToSharedRef());

	if (PlayerOwner)
	{
		PlayerOwner->bShowMouseCursor = true;
		PlayerOwner->SetInputMode(FInputModeUIOnly());
		PlayerOwner->SetPause(true);
	}

	bEndgameMenuVisible = true;
}

void ADefenseHUD::HideEndgameMenu()
{
	if (!bEndgameMenuVisible || !GEngine || !GEngine->GameViewport) return;

	if (EndgameMenuContainer.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(EndgameMenuContainer.ToSharedRef());
	}
	EndgameMenuContainer.Reset();
	EndgameMenuWidget.Reset();

	if (PlayerOwner)
	{
		PlayerOwner->bShowMouseCursor = false;
		PlayerOwner->SetInputMode(FInputModeGameOnly());
		PlayerOwner->SetPause(false);
	}

	bEndgameMenuVisible = false;
}

void ADefenseHUD::SetWave(int32 NewWave)
{
	CurrentWave = NewWave;
}

void ADefenseHUD::SetScore(int32 NewScore)
{
	CurrentScore = NewScore;
}

