// DefenseHUD.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DefenseHUD.generated.h"

class SSettingsWidget;
class SWeakWidget;
class SMainMenuWidget; // <-- forward declare
class SEndgameMenuWidget;  // forward declare

UCLASS()
class FPS_MDD_API ADefenseHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

	// Health hook
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetBaseHealth(float InCurrent, float InMax);

	// EndLevel UI
	UFUNCTION(BlueprintCallable, Category = "HUD") void ShowEndLevelButton();
	UFUNCTION(BlueprintCallable, Category = "HUD") void HideEndLevelButton();
	UFUNCTION(BlueprintCallable, Category = "HUD") void ToggleEndLevelButton();
	UFUNCTION(BlueprintPure, Category = "HUD") bool IsEndLevelButtonVisible() const { return bEndButtonVisible; }

	// Settings (pause) menu – call this from input (Esc), not automatically on BeginPlay.
	void ShowSettingsMenu();
	void HideSettingsMenu();

	// Main menu (dedicated MainMenu level only)
	void ShowMainMenu();
	void HideMainMenu();

	void ToggleSettingsMenu();      
	bool IsMainMenuVisible() const { return bMainMenuVisible; }

	void ShowEndgameMenu();
	void HideEndgameMenu();
	bool IsEndgameMenuVisible() const { return bEndgameMenuVisible; }

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetWave(int32 InWave) { CurrentWave = FMath::Max(1, InWave); }//print wave number on screen

	// --- Crosshair options ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
	bool bShowCrosshair = true;

	// Size of each arm of the crosshair in pixels (only for line-drawn mode)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair", meta = (ClampMin = "2.0", ClampMax = "100.0"))
	float CrosshairArm = 10.f;

	// Gap around the center (line-drawn mode)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair", meta = (ClampMin = "0.0", ClampMax = "50.0"))
	float CrosshairGap = 4.f;

	// Line thickness (line-drawn mode)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair", meta = (ClampMin = "0.5", ClampMax = "8.0"))
	float CrosshairThickness = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
	FLinearColor CrosshairColor = FLinearColor::White;

	// Optional: assign a texture to use a PNG reticle instead of lines
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
	TObjectPtr<UTexture2D> CrosshairTexture = nullptr;

	// Scale the texture (if using CrosshairTexture)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair", meta = (ClampMin = "0.05", ClampMax = "5.0"))
	float CrosshairTexScale = 1.0f;

	// Helper to enable/disable from other classes (e.g., while a menu is open)
	UFUNCTION(BlueprintCallable, Category = "Crosshair")
	void SetCrosshairVisible(bool bVisible) { bShowCrosshair = bVisible; }

	UFUNCTION(BlueprintCallable, Category = "HUD") //score UFUNCTION
	void SetScore(int32 NewScore);

private:
	// Health bar data
	float CurrentHealth = 100.f;
	float MaxHealth = 100.f;
	// --- Score ---
	int32 CurrentScore = 0;

	int32 CurrentWave = 1; //wave number

	FVector2D BarPos = FVector2D(50.f, 50.f);
	FVector2D BarSize = FVector2D(300.f, 24.f);

	// Settings/pause tracking
	TSharedPtr<SSettingsWidget> SettingsWidget;
	TSharedPtr<SWeakWidget>     SlateWidgetContainer;
	bool bSettingsVisible = false;

	// EndLevel button
	TSharedPtr<SWidget> EndLevelWidget;
	bool bEndButtonVisible = false;
	FReply OnEndLevelClicked();

	// Main menu tracking
	TSharedPtr<SMainMenuWidget> MainMenuWidget;
	TSharedPtr<SWeakWidget>     MainMenuContainer;
	bool bMainMenuVisible = false;

	//EndgameMenu
	TSharedPtr<SEndgameMenuWidget> EndgameMenuWidget;
	TSharedPtr<class SWeakWidget>  EndgameMenuContainer;
	bool bEndgameMenuVisible = false;

	// End screen flag
	bool bShowGameEnded = false;

	// Draw helpers
	void DrawRectFilled(const FLinearColor& Color, const FVector2D& Pos, const FVector2D& Size);

};
