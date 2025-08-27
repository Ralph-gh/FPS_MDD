// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/DefenseBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/HealthComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "HUD/DefenseHUD.h"

ADefenseBase::ADefenseBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(Root);
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BaseMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
}

void ADefenseBase::BeginPlay()
{
	Super::BeginPlay();
	UpdateHUD(); // push initial values to HUD at game start
}
void ADefenseBase::UpdateHUD() const
{
	if (!Health) return;

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (ADefenseHUD* HUD = Cast<ADefenseHUD>(PC->GetHUD()))
		{
			HUD->SetBaseHealth(Health->CurrentHealth, Health->MaxHealth);
		}
	}
}

void ADefenseBase::BaseSetMaxHealth(float NewMax)
{
	if (!Health) return;
	Health->SetMaxHealth(NewMax);
	UpdateHUD();
}

void ADefenseBase::BaseHeal(float Amount)
{
	if (!Health) return;
	Health->Heal(Amount);
	UpdateHUD();
}

void ADefenseBase::BaseDamage(float Amount)
{
	if (!Health) return;
	Health->Damage(Amount);
	UpdateHUD();
}