// SSettingsWidget.cpp
#include "GUI/Slate/SSettingsWidget.h"
#include "HUD/DefenseHUD.h"
#include "SlateOptMacros.h"

#include "Widgets/SOverlay.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "Styling/CoreStyle.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SSettingsWidget::Construct(const FArguments& InArgs)
{
	OwnerHUD = InArgs._OwnerHUD;

	const FMargin ContentPadding(500.f, 300.f);
	const FMargin ButtonPadding(10.f);

	// Use a robust font getter that always exists
	FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Regular", 45);
	FSlateFontInfo ButtonFont = FCoreStyle::GetDefaultFontStyle("Regular", 40);

	ChildSlot
		[
			SNew(SOverlay)

				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SImage)
						.Image(FCoreStyle::Get().GetBrush("WhiteBrush"))
						.ColorAndOpacity(FLinearColor(0, 0, 0, 0.8f))
				]

				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(ContentPadding)
				[
					SNew(SVerticalBox)

						// Title
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(ButtonPadding)
						[
							SNew(STextBlock)
								.Font(TitleFont)
								.Text(FText::FromString(TEXT("Pause Menu")))
								.Justification(ETextJustify::Center)
						]

						// Resume
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(ButtonPadding)
						[
							SNew(SButton)
								.OnClicked(this, &SSettingsWidget::OnResumeClicked)
								[
									SNew(STextBlock)
										.Font(ButtonFont)
										.Text(FText::FromString(TEXT("Resume")))
								]
						]

					// Settings
					+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(ButtonPadding)
						[
							SNew(SButton)
								.OnClicked(this, &SSettingsWidget::OnSettingsClicked)
								[
									SNew(STextBlock)
										.Font(ButtonFont)
										.Text(FText::FromString(TEXT("Settings")))
								]
						]

					// Quit
					+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(ButtonPadding)
						[
							SNew(SButton)
								.OnClicked(this, &SSettingsWidget::OnQuitClicked)
								[
									SNew(STextBlock)
										.Font(ButtonFont)
										.Text(FText::FromString(TEXT("Quit")))
								]
						]
				]
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply SSettingsWidget::OnResumeClicked()
{
	UE_LOG(LogTemp, Display, TEXT("Resume Clicked"));
	if (OwnerHUD.IsValid())
	{
		OwnerHUD->HideSettingsMenu();
		UE_LOG(LogTemp, Warning, TEXT("Settings Menu hidden now"));
	}
	return FReply::Handled();
}

FReply SSettingsWidget::OnSettingsClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Settings Clicked"));
	return FReply::Handled();
}

FReply SSettingsWidget::OnQuitClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Quit Clicked"));
	return FReply::Handled();
}
