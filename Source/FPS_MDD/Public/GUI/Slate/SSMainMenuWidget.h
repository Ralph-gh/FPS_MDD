#pragma once
#include "Widgets/SCompoundWidget.h"

class ADefenseHUD;

class SMainMenuWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMainMenuWidget) {}
        SLATE_ARGUMENT(TWeakObjectPtr<ADefenseHUD>, OwnerHUD)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    FReply OnPlayClicked();
    FReply OnQuitClicked();

    TWeakObjectPtr<ADefenseHUD> OwnerHUD;
};
