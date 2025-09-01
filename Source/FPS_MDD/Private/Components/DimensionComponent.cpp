#include "Components/DimensionComponent.h"
#include "AFPSProjectGameModeBase.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

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
    if (UWorld* W = GetWorld())
    {
        if (AFPSProjectGameModeBase* GM = W->GetAuthGameMode<AFPSProjectGameModeBase>())
        {
            CachedGM = GM;
            GM->RegisterDimensionComponent(this);

            // Immediately conform to current dimension on spawn
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

    Owner->SetActorHiddenInGame(!bVisible);

    if (bDisableCollisionAndTickWhenHidden)
    {
        Owner->SetActorEnableCollision(bVisible);
        Owner->SetActorTickEnabled(bVisible);
    }
}
