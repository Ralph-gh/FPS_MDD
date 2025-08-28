#include "GUI/Slate/SEndgameMenuWidget.h"
#include "HUD/DefenseHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Styling/CoreStyle.h"

#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBox.h"
//#include "Widgets/Layout/SVerticalBox.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"

void SEndgameMenuWidget::Construct(const FArguments& InArgs)
{
    OwnerHUD = InArgs._OwnerHUD;

    FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Regular", 40);
    FSlateFontInfo ButtonFont = FCoreStyle::GetDefaultFontStyle("Regular", 32);

    ChildSlot
        [
            SNew(SOverlay)

                + SOverlay::Slot()
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                [
                    SNew(SBox).WidthOverride(540.f).HeightOverride(420.f)
                        [
                            SNew(SBorder)
                                .BorderBackgroundColor(FLinearColor(0, 0, 0, 0.6f))
                                .Padding(24.f)
                                [
                                    SNew(SVerticalBox)

                                        // Title
                                        + SVerticalBox::Slot().AutoHeight().Padding(6.f)
                                        [
                                            SNew(STextBlock)
                                                .Text(FText::FromString("Game Over"))
                                                .Font(TitleFont)
                                                .Justification(ETextJustify::Center)
                                        ]

                                        // Main Menu
                                        + SVerticalBox::Slot().AutoHeight().Padding(10.f)
                                        [
                                            SNew(SBox).WidthOverride(360.f).HeightOverride(72.f)
                                                [
                                                    SNew(SButton)
                                                        .OnClicked(this, &SEndgameMenuWidget::OnMainMenuClicked)
                                                        [
                                                            SNew(STextBlock).Text(FText::FromString("Main Menu"))
                                                                .Font(ButtonFont)
                                                                .Justification(ETextJustify::Center)
                                                        ]
                                                ]
                                        ]

                                    // Restart Level1
                                    + SVerticalBox::Slot().AutoHeight().Padding(10.f)
                                        [
                                            SNew(SBox).WidthOverride(360.f).HeightOverride(72.f)
                                                [
                                                    SNew(SButton)
                                                        .OnClicked(this, &SEndgameMenuWidget::OnRestartClicked)
                                                        [
                                                            SNew(STextBlock).Text(FText::FromString("Restart"))
                                                                .Font(ButtonFont)
                                                                .Justification(ETextJustify::Center)
                                                        ]
                                                ]
                                        ]

                                    // Quit
                                    + SVerticalBox::Slot().AutoHeight().Padding(10.f)
                                        [
                                            SNew(SBox).WidthOverride(360.f).HeightOverride(72.f)
                                                [
                                                    SNew(SButton)
                                                        .OnClicked(this, &SEndgameMenuWidget::OnQuitClicked)
                                                        [
                                                            SNew(STextBlock).Text(FText::FromString("Quit"))
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

FReply SEndgameMenuWidget::OnMainMenuClicked()
{
    if (!OwnerHUD.IsValid()) return FReply::Handled();
    OwnerHUD->HideEndgameMenu();
    UGameplayStatics::OpenLevel(OwnerHUD.Get(), FName(TEXT("/Game/Maps/MainMenu")));
    return FReply::Handled();
}

FReply SEndgameMenuWidget::OnRestartClicked()
{
    if (!OwnerHUD.IsValid()) return FReply::Handled();
    OwnerHUD->HideEndgameMenu();
    UGameplayStatics::OpenLevel(OwnerHUD.Get(), FName(TEXT("/Game/Maps/Level1")));
    return FReply::Handled();
}

FReply SEndgameMenuWidget::OnQuitClicked()
{
    if (!OwnerHUD.IsValid()) return FReply::Handled();
    UKismetSystemLibrary::QuitGame(
        OwnerHUD.Get(), OwnerHUD->GetOwningPlayerController(),
        EQuitPreference::Quit, false);
    return FReply::Handled();
}
