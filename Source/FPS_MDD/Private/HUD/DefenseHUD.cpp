// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/DefenseHUD.h"
#include "Engine/Canvas.h"
#include "CanvasItem.h"

void ADefenseHUD::SetBaseHealth(float InCurrent, float InMax)
{
	MaxHealth = FMath::Max(1.f, InMax);
	CurrentHealth = FMath::Clamp(InCurrent, 0.f, MaxHealth);
}

void ADefenseHUD::DrawHUD()
{
	Super::DrawHUD();


	UE_LOG(LogTemp, Warning, TEXT("ADefenseHUD::DrawHUD() called"));

	if (Canvas && GEngine)
	{
		// Simple visible text
		Canvas->DrawText(
			GEngine->GetLargeFont(),           // use bigger font so you see it
			TEXT("HELLO FROM HUD"),
			300.f, 300.f                       // screen X,Y
		);

		// On-screen debug message too
		GEngine->AddOnScreenDebugMessage(
			-1, 1.5f, FColor::Red, TEXT("HUD is alive")
		);
	}
	DrawHealthBar();
}

void ADefenseHUD::DrawHealthBar()
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
}

void ADefenseHUD::DrawRectFilled(const FLinearColor& Color, const FVector2D& Pos, const FVector2D& Size)
{
	FCanvasTileItem Tile(Pos, Size, Color);
	Tile.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Tile);
}
