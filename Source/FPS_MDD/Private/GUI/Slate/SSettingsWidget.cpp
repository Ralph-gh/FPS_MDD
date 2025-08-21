// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/Slate/SSettingsWidget.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SSettingsWidget::Construct(const FArguments& InArgs)
{
	//Padding
	const FMargin ContentPadding = FMargin(500.0f, 300.0f);
	const FMargin ButtonPadding = FMargin(10.0f);
	

	//Title
	FSlateFontInfo TitleTextStyle = FCoreStyle::Get().GetFontStyle("EmbassedText");
	TitleTextStyle.Size = 45.0f;

	const FText TitleText = FText::FromString("Pause Menu");

	//Buttons
	FSlateFontInfo ButtonTextStyle = TitleTextStyle;
	ButtonTextStyle.Size = 40.0f;
	
	const FText ResumeText = FText::FromString("Resume");
	const FText SettingsText = FText::FromString("Settings");
	const FText QuitText = FText::FromString("Quit");

	ChildSlot
			[
			SNew(SOverlay)

			+SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[

			SNew(SImage)
				.Image(FCoreStyle::Get().GetBrush("WhiteBrush"))
				.ColorAndOpacity(FLinearColor(0, 0, 0, 0.8f))
			]

			//Content
		+SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(ContentPadding)
		[	
			SNew(SVerticalBox)

				+ SVerticalBox::Slot().AutoHeight().Padding(ButtonPadding)

				[
					SNew(STextBlock)
						.Font(TitleTextStyle)
						.Text(TitleText)
						.Justification(ETextJustify::Center)
				]
				//Resume Button
				+ SVerticalBox::Slot().AutoHeight().Padding(ButtonPadding)
					[
					SNew(SButton)
					.OnClicked(this,&SSettingsWidget::OnResumeClicked )
							[
							SNew(STextBlock)
							.Font(ButtonTextStyle)
							.Text(ResumeText)
							.Justification(ETextJustify::Center)
					]

				]
					//Settings Button
			+ SVerticalBox::Slot().AutoHeight().Padding(ButtonPadding)
				[
					SNew(SButton)
					.OnClicked(this,&SSettingsWidget::OnSettingsClicked)
				[
					SNew(STextBlock)
					.Font(ButtonTextStyle)
					.Text(SettingsText)
					.Justification(ETextJustify::Center)
				]
				]
					//Quit Button

			+ SVerticalBox::Slot().AutoHeight().Padding(ButtonPadding)
				[
					SNew(SButton)
					.OnClicked(this,&SSettingsWidget::OnQuitClicked)
				[
					SNew(STextBlock)
					.Font(ButtonTextStyle)
					.Text(QuitText)
					.Justification(ETextJustify::Center)]
				]
		]
	
	
	
		
	];
	
}
FReply SSettingsWidget::OnResumeClicked() const
{
	UE_LOG(LogTemp, Warning, TEXT("Resume Clicked"));
	return FReply::Handled();
}
FReply SSettingsWidget::OnSettingsClicked() const
{
	UE_LOG(LogTemp, Warning, TEXT("Settings Clicked"));
	return FReply::Handled();
}
FReply SSettingsWidget::OnQuitClicked() const
{
	UE_LOG(LogTemp, Warning, TEXT("Quit Clicked"));
	return FReply::Handled();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
