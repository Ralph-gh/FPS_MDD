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

	const bool bVisible = (Active == Dimension);

	// 1) Visuals only (always hide when not active)
	Owner->SetActorHiddenInGame(!bVisible);

	// 2) Optional: collision/tick control (both default OFF so invisible actors still interact)
	if (bAffectCollisionWhenHidden)
	{
		Owner->SetActorEnableCollision(bVisible);
	}
	if (bAffectTickWhenHidden)
	{
		Owner->SetActorTickEnabled(bVisible);
	}
}
