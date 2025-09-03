
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DimensionComponent.generated.h"

// ---------- Dimension type (scoped enum) ----------
UENUM(BlueprintType)
enum class EDefenseDimension : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Shadow UMETA(DisplayName = "Shadow")
};

class AFPSProjectGameModeBase; // forward declare (avoid circular include)

/**
 * Attach this to any actor that should appear only in a specific "dimension".
 * GameMode is the central manager: it tracks the active dimension and tells
 * registered components to update visibility/collision/tick.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPS_MDD_API UDimensionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDimensionComponent();

	// Which dimension this actor belongs to
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimension")
	EDefenseDimension Dimension = EDefenseDimension::Normal;

	// When not in the active dimension, also disable collision & tick (recommended)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimension")
	bool bDisableCollisionAndTickWhenHidden = true;

	// When hidden (inactive dimension), should we also disable collision?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimension")
	bool bAffectCollisionWhenHidden = false;   // <-- default false for your case

	// When hidden, should we also disable Tick?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimension")
	bool bAffectTickWhenHidden = false;        // <-- default false for your case

	// Called by the manager (GameMode) when the active dimension changes
	void ApplyForDimension(EDefenseDimension Active) const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	// Cached GM so we can unregister cleanly
	TWeakObjectPtr<AFPSProjectGameModeBase> CachedGM;

	// Internal registration helpers
	void RegisterWithManager();
	void UnregisterFromManager();
};
