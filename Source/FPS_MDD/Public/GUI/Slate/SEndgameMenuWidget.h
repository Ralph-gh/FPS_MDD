#pragma once
#include "Widgets/SCompoundWidget.h"

class ADefenseHUD;

class SEndgameMenuWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SEndgameMenuWidget) {}
        SLATE_ARGUMENT(TWeakObjectPtr<ADefenseHUD>, OwnerHUD)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    FReply OnMainMenuClicked();
    FReply OnRestartClicked();
    FReply OnQuitClicked();

    TWeakObjectPtr<ADefenseHUD> OwnerHUD;
};
