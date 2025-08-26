// DefenseHUD.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DefenseHUD.generated.h"

class SSettingsWidget;
class SWeakWidget;

UCLASS()
class FPS_MDD_API ADefenseHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

	bool bShowGameEnded = false;//flag for Endgame

	// Health hook
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetBaseHealth(float InCurrent, float InMax);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowEndLevelButton();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void HideEndLevelButton();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ToggleEndLevelButton();

	UFUNCTION(BlueprintPure, Category = "HUD")
	bool IsEndLevelButtonVisible() const { return bEndButtonVisible; }

	// Pause menu
	void ShowSettingsMenu();
	void HideSettingsMenu();
	

private:
	// Health bar data
	float CurrentHealth = 100.f;
	float MaxHealth = 100.f;
	FVector2D BarPos = FVector2D(50.f, 50.f);
	FVector2D BarSize = FVector2D(300.f, 24.f);

	// Slate menu tracking
	TSharedPtr<SSettingsWidget> SettingsWidget;
	TSharedPtr<SWeakWidget>     SlateWidgetContainer;
	bool bSettingsVisible = false;
	//EndLevel
	TSharedPtr<SWidget> EndLevelWidget;
	bool bEndButtonVisible = false;
	
	FReply OnEndLevelClicked();

	// Draw helpers
	void DrawRectFilled(const FLinearColor& Color, const FVector2D& Pos, const FVector2D& Size);
};
