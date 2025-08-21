// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/Slate/SSettingsWidget.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SSettingsWidget::Construct(const FArguments& InArgs)
{
	
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
		[	
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
				[
					SNew(STextBlock)
				]

		]
	

		
	];
	
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
