// SSettingsWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class ADefenseHUD; // forward declare to avoid circular include

class FPS_MDD_API SSettingsWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSettingsWidget) {}
		SLATE_ARGUMENT(TWeakObjectPtr<ADefenseHUD>, OwnerHUD)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	TWeakObjectPtr<ADefenseHUD> OwnerHUD;

	FReply OnResumeClicked();
	FReply OnSettingsClicked();
	FReply OnQuitClicked();
};
