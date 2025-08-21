// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GUI/Slate/SSettingsWidget.h"
#include "DefenseHUD.generated.h"

/**
 * 
 */
UCLASS()
class FPS_MDD_API ADefenseHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;
	virtual void BeginPlay() override;
	// We'll call this from gameplay code later
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetBaseHealth(float InCurrent, float InMax);
	//UTexture2D* CrosshairTexture;

	//Slates Method of Making UI
	TSharedPtr<class SSettingsWidget> SettingsWidget;
	TSharedPtr<class SWidget> SlateWidgetContainer;

	void ShowSettingsMenu();   //Used to show settings menu
	void HideSettingsMenu();

private:
	// Temporary demo values
	float CurrentHealth = 100.f;
	float MaxHealth = 100.f;

	// Layout
	FVector2D BarPos = FVector2D(50.f, 50.f);
	FVector2D BarSize = FVector2D(300.f, 24.f);
	
	//void DrawHealthBar();
	void DrawRectFilled(const FLinearColor& Color, const FVector2D& Pos, const FVector2D& Size);
};

