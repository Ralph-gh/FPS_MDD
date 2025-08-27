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
    {
        OwnerHUD = InArgs._OwnerHUD;

        // Bigger font
        FSlateFontInfo ButtonFont = FCoreStyle::GetDefaultFontStyle("Regular", 36); // size 36, increase as needed

        ChildSlot
            [
                SNew(SOverlay)

                    + SOverlay::Slot()
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SBox)
                            .WidthOverride(500.f)   // overall menu box width
                            .HeightOverride(400.f)  // overall height
                            [
                                SNew(SBorder)
                                    .Padding(30.f)
                                    .BorderBackgroundColor(FLinearColor(0, 0, 0, 0.6f))
                                    [
                                        SNew(SVerticalBox)

                                            // Play button
                                            + SVerticalBox::Slot()
                                            .AutoHeight().Padding(15)
                                            [
                                                SNew(SBox)
                                                    .WidthOverride(300.f)    // button width
                                                    .HeightOverride(80.f)    // button height
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

                                        // Quit button
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
                    ]
            ];
    }
}

FReply SMainMenuWidget::OnPlayClicked()
{
    if (!OwnerHUD.IsValid()) return FReply::Handled();

    // unpause & hide menu first
    OwnerHUD->HideMainMenu();

    // Load your gameplay map (rename as needed)
    UGameplayStatics::OpenLevel(OwnerHUD.Get(), FName(TEXT("/Game/Maps/Arena01")));
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