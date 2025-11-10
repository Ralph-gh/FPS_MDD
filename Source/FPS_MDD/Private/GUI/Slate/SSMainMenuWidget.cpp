// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/Slate/SSMainMenuWidget.h"
#include "SlateOptMacros.h"

#include "HUD/DefenseHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
//#include "Widgets/Layout/SVerticalBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"

void SMainMenuWidget::Construct(const FArguments& InArgs)
{
    OwnerHUD = InArgs._OwnerHUD;

    // Bigger font for buttons (same as you had)
    FSlateFontInfo ButtonFont = FCoreStyle::GetDefaultFontStyle("Regular", 36);

    // ----- LEFT: your existing menu panel (unchanged layout) -----
    TSharedRef<SWidget> LeftMenuPanel =
        SNew(SOverlay)
        + SOverlay::Slot()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        [
            SNew(SBox)
                .WidthOverride(500.f)
                .HeightOverride(400.f)
                [
                    SNew(SBorder)
                        .Padding(30.f)
                        .BorderBackgroundColor(FLinearColor(0, 0, 0, 0.6f))
                        [
                            SNew(SVerticalBox)

                                // Play
                                + SVerticalBox::Slot()
                                .AutoHeight().Padding(15)
                                [
                                    SNew(SBox)
                                        .WidthOverride(300.f)
                                        .HeightOverride(80.f)
                                        [
                                            SNew(SButton)
                                                .OnClicked(this, &SMainMenuWidget::OnPlayClicked)
                                                [
                                                    SNew(STextBlock)
                                                        .Text(FText::FromString("Play"))
                                                        .Font(ButtonFont)
                                                        .Justification(ETextJustify::Center)
                                                ]
                                        ]
                                ]

                            // Quit
                            + SVerticalBox::Slot()
                                .AutoHeight().Padding(15)
                                [
                                    SNew(SBox)
                                        .WidthOverride(300.f)
                                        .HeightOverride(80.f)
                                        [
                                            SNew(SButton)
                                                .OnClicked(this, &SMainMenuWidget::OnQuitClicked)
                                                [
                                                    SNew(STextBlock)
                                                        .Text(FText::FromString("Quit"))
                                                        .Font(ButtonFont)
                                                        .Justification(ETextJustify::Center)
                                                ]
                                        ]
                                ]
                        ]
                ]
        ];

    // ----- RIGHT: instructions panel (your exact text) -----
    TSharedRef<SWidget> RightInstructionsPanel =
        SNew(SBorder)
        // Light background so black text reads well
        .BorderBackgroundColor(FLinearColor(1.f, 1.f, 1.f, 0.55f))
        .Padding(FMargin(12.f))
        [
            SNew(SScrollBox)
                + SScrollBox::Slot()
                [
                    SNew(STextBlock)
                        .AutoWrapText(true)
                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
                        // Make text black
                        .ColorAndOpacity(FSlateColor(FLinearColor::Black))
                        // Optional: tiny shadow for readability
                        .ShadowOffset(FVector2D(0.8f, 0.8f))
                        .ShadowColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.25f))
                        .Text(FText::FromString(
                            TEXT("Objective\n")
                            TEXT("Defend the Base against incoming waves. Survive all waves to win. If the Base’s health hits zero, the game ends.\n\n")

                            TEXT("Controls\n")
                            TEXT("W / A / S / D: Move\n")
                            TEXT("Mouse: Look\n")
                            TEXT("Space: Jump\n")
                            TEXT("Left-Click: Fire projectile\n\n")

                            TEXT("Your Weapons\n")
                            TEXT("Primary Fire: Fast projectile that destroys on impact.\n\n")

                            TEXT("Base & HUD\n")
                            TEXT("Base Health Bar appears at the top-left. Keep it above zero to stay alive.\n")
                            TEXT("The EndLevel button appears when the level is complete (or as enabled by the designer) to proceed.\n\n")

                            TEXT("Waves & Spawners\n")
                            TEXT("Wave 1: Clear existing enemies.\n")
                            TEXT("Wave 2: Spawner appears; it stops after spawning 20 enemies and waits for you to clear them.\n")
                            TEXT("Wave 3: More spawner; each stops at 20 enemies and waits for a clear.\n")
                            TEXT("Waves keeps appearing, try to survive and get the highest score possible\n\n")

                            TEXT("Enemies\n")
                            TEXT("Spawn above the base and fly towards it.\n")
                            TEXT("Some variants are “shadow” types—harder to track; prioritize by proximity to the Base.\n")
                            TEXT("Press V to switch between Normal vision and shadow vision\n\n")

                            TEXT("Win / Lose\n")
                            TEXT("Win: Survive as long as possible\n")
                            TEXT("Lose: Base Health reaches 0 (Game Over).\n\n")

                            TEXT("Tips\n")
                            TEXT("Aim down sights with the camera center; your bullet line traces where you look.\n")
                            TEXT("Clear high-threat enemies first (closest to Base).\n")
                        ))
                ]
        ];

    // ----- Layout: left menu + right instructions side-by-side -----
    ChildSlot
        [
            SNew(SHorizontalBox)

                // left (your current menu)
                + SHorizontalBox::Slot()
                .FillWidth(0.40f)
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                [
                    LeftMenuPanel
                ]

                // right (instructions)
                + SHorizontalBox::Slot()
                .FillWidth(0.60f)
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Fill)
                .Padding(FMargin(12.f, 24.f, 24.f, 24.f))
                [
                    RightInstructionsPanel
                ]
        ];
}


FReply SMainMenuWidget::OnPlayClicked()
{
    if (!OwnerHUD.IsValid()) return FReply::Handled();

    // unpause & hide menu first
    OwnerHUD->HideMainMenu();

    // Load your gameplay map (rename as needed)
    UGameplayStatics::OpenLevel(OwnerHUD.Get(), FName(TEXT("/Game/Maps/Level1")));
    return FReply::Handled();
}

FReply SMainMenuWidget::OnQuitClicked()
{
    if (!OwnerHUD.IsValid()) return FReply::Handled();

    UKismetSystemLibrary::QuitGame(
        OwnerHUD.Get(),
        OwnerHUD->GetOwningPlayerController(),
        EQuitPreference::Quit, false
    );
    return FReply::Handled();
}