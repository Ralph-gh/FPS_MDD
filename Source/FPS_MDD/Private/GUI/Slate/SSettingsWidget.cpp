// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/Slate/SSettingsWidget.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SSettingsWidget::Construct(const FArguments& InArgs)
{
	//Padding
	const FMargin ContentPadding = FMargin(500.0f, 300.0f);

	//Title
	FSlateFontInfo TitleTextStyle = FCoreStyle::Get().GetFontStyle("EmbassedText");
	TitleTextStyle.Size = 45.0f;

	const FText TitleText = FText::FromString("Settings Menu");


	ChildSlot
			[
			SNew(SOverlay)
			+SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[

			SNew(SImage)
				.ColorAndOpacity(FColor::Black)

		]
		+SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(ContentPadding)
		[	
			SNew(SVerticalBox)
			+SVerticalBox::Slot()

				[
					SNew(STextBlock)
						.Font(TitleTextStyle)
						.Text(TitleText)
						.Justification(ETextJustify::Center)
				]
					/*//Resume Button
					+SVerticalBox::Slot();
						SNew(SButton)
					[
						SNew(STextBlock)
							.Font(TitleTextStyle)
							.Text(TitleText)
							.Justification(ETextJustify::Center)
					]

		]
			//Settings Button
			+ SVerticalBox::Slot();
			SNew(SButton)
				[
					SNew(STextBlock)
						.Font(TitleTextStyle)
						.Text(TitleText)
						.Justification(ETextJustify::Center)
				]
					]
				]*/
		]
	

		
	];
	
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
