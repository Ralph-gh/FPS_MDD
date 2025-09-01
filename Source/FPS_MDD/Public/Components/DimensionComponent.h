#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DimensionComponent.generated.h"

// ---------- Dimension type (kept here for simplicity) ----------
UENUM(BlueprintType)
enum class EDefenseDimension : uint8
{
    Normal UMETA(DisplayName = "Normal"),
    Shadow UMETA(DisplayName = "Shadow")
};

class AFPSProjectGameModeBase; // forward declare, avoid circular include

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPS_MDD_API UDimensionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDimensionComponent();

    // Which “dimension” this actor belongs to
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimension")
    EDefenseDimension Dimension = EDefenseDimension::Normal;

    // When hidden (not the active dimension), also disable collision & tick
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimension")
    bool bDisableCollisionAndTickWhenHidden = true;

    // Called by the manager (GameMode) when the active dimension changes
    void ApplyForDimension(EDefenseDimension Active) const;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    // Keep a weak ref to GameMode so we can unregister on EndPlay
    TWeakObjectPtr<AFPSProjectGameModeBase> CachedGM;

    // Internal registration helpers
    void RegisterWithManager();
    void UnregisterFromManager();
};
