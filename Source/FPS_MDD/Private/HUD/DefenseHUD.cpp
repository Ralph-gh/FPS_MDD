// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/DefenseHUD.h"
#include "GUI/Slate/SSettingsWidget.h"
#include "Widgets/SWeakWidget.h"
#include "Engine/Canvas.h"
#include "CanvasItem.h"

void ADefenseHUD::SetBaseHealth(float InCurrent, float InMax)
{
	MaxHealth = FMath::Max(1.f, InMax);
	CurrentHealth = FMath::Clamp(InCurrent, 0.f, MaxHealth);

	

}
void ADefenseHUD::BeginPlay()
{
	Super::BeginPlay();

	ShowSettingsMenu();
}


void ADefenseHUD::ShowSettingsMenu()
{
	SettingsWidget = SNew(SSettingsWidget);
	GEngine-> GameViewport->AddViewportWidgetContent(SAssignNew(SlateWidgetContainer, SWeakWidget).PossiblyNullContent(SettingsWidget.ToSharedRef()));

	PlayerOwner->bShowMouseCursor = true;
	PlayerOwner->SetInputMode(FInputModeUIOnly());


}

void ADefenseHUD::HideSettingsMenu()
{
	GEngine->GameViewport->AddViewportWidgetContent(SAssignNew(SlateWidgetContainer, SWeakWidget).PossiblyNullContent(SettingsWidget.ToSharedRef()));

	PlayerOwner->bShowMouseCursor = false;
	PlayerOwner->SetInputMode(FInputModeUIOnly());
}

void ADefenseHUD::DrawHUD()
{
	Super::DrawHUD();


	UE_LOG(LogTemp, Warning, TEXT("ADefenseHUD::DrawHUD() called"));

	if (!Canvas) return;

	const float ratio = FMath::Clamp(CurrentHealth / FMath::Max(1.f, MaxHealth), 0.f, 1.f);

	// background
	DrawRectFilled(FLinearColor(0.f, 0.f, 0.f, 0.6f), BarPos, BarSize);

	// fill
	DrawRectFilled(FLinearColor(0.1f, 0.8f, 0.2f, 0.9f),
		BarPos, FVector2D(BarSize.X * ratio, BarSize.Y));

	// label (big so you notice it)

	if (GEngine)
	{
		Canvas->DrawText(GEngine->GetLargeFont(),
			FString::Printf(TEXT("Base %d / %d"),
				FMath::RoundToInt(CurrentHealth), FMath::RoundToInt(MaxHealth)),
			BarPos.X, BarPos.Y - 32.f);
	}
	//if (!CrosshairTexture) return;


	//DrawHealthBar();
}

/*void ADefenseHUD::DrawHealthBar()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (ADefenseHUD* HUD = Cast<ADefenseHUD>(PC->GetHUD()))
		{
			HUD->SetBaseHealth(65.f, 100.f); // you should see ~65% bar if you added the bar code
		}
	}

	if (!Canvas || MaxHealth <= 0.f) return;

	const float Ratio = FMath::Clamp(CurrentHealth / MaxHealth, 0.f, 1.f);

	// Background
	DrawRectFilled(FLinearColor(0.f, 0.f, 0.f, 0.6f), BarPos, BarSize);

	// Fill
	const FVector2D FillSize(BarSize.X * Ratio, BarSize.Y);
	DrawRectFilled(FLinearColor(0.1f, 0.8f, 0.2f, 0.9f), BarPos, FillSize);

	// Border (simple 4 lines using filled rects)
	DrawRectFilled(FLinearColor::White, BarPos, FVector2D(BarSize.X, Border));                                // Top
	DrawRectFilled(FLinearColor::White, FVector2D(BarPos.X, BarPos.Y + BarSize.Y - Border), FVector2D(BarSize.X, Border)); // Bottom
	DrawRectFilled(FLinearColor::White, BarPos, FVector2D(Border, BarSize.Y));                                // Left
	DrawRectFilled(FLinearColor::White, FVector2D(BarPos.X + BarSize.X - Border, BarPos.Y), FVector2D(Border, BarSize.Y)); // Right

	// Optional label
	const FString Label = FString::Printf(TEXT("Base %d / %d"),
		FMath::RoundToInt(CurrentHealth), FMath::RoundToInt(MaxHealth));
	FCanvasTextItem TextItem(FVector2D(BarPos.X, BarPos.Y - 18.f), FText::FromString(Label), GEngine->GetSmallFont(), FLinearColor::White);
	TextItem.EnableShadow(FLinearColor::Black);
	Canvas->DrawItem(TextItem);
}*/


void ADefenseHUD::DrawRectFilled(const FLinearColor& Color, const FVector2D& Pos, const FVector2D& Size)
{
	FCanvasTileItem Tile(Pos, Size, Color);
	Tile.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Tile);
}
