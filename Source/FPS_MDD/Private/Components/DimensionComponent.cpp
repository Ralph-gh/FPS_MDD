// Source/<YourModule>/Private/Components/DimensionComponent.cpp
#include "Components/DimensionComponent.h"
#include "AFPSProjectGameModeBase.h"
#include "GameFramework/Actor.h"

UDimensionComponent::UDimensionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDimensionComponent::BeginPlay()
{
	Super::BeginPlay();
	RegisterWithManager();
}

void UDimensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterFromManager();
	Super::EndPlay(EndPlayReason);
}

void UDimensionComponent::RegisterWithManager()
{
	if (UWorld* World = GetWorld())
	{
		if (AFPSProjectGameModeBase* GM = World->GetAuthGameMode<AFPSProjectGameModeBase>())
		{
			CachedGM = GM;
			GM->RegisterDimensionComponent(this);

			// Conform immediately to the current dimension
			ApplyForDimension(GM->GetCurrentDimension());
		}
	}
}

void UDimensionComponent::UnregisterFromManager()
{
	if (CachedGM.IsValid())
	{
		CachedGM->UnregisterDimensionComponent(this);
	}
}

void UDimensionComponent::ApplyForDimension(EDefenseDimension Active) const
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	const bool bShouldBeVisible = (Active == Dimension);

	// Visual visibility
	Owner->SetActorHiddenInGame(!bShouldBeVisible);

	// Optional performance/interaction gating
	if (bDisableCollisionAndTickWhenHidden)
	{
		Owner->SetActorEnableCollision(bShouldBeVisible);
		Owner->SetActorTickEnabled(bShouldBeVisible);
	}
}
