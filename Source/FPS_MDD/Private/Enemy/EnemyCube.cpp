// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyCube.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AEnemyCube::AEnemyCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the cube mesh and set it as root
	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	RootComponent = CubeMesh;

	// Load the default engine cube mesh so you see something in-game
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeAsset.Succeeded())
	{
		CubeMesh->SetStaticMesh(CubeAsset.Object);
	}

	CubeMesh->OnComponentHit.AddDynamic(this, &AEnemyCube::OnHit);
	CubeMesh->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCube::OnOverlap);
}

// Called when the game starts or when spawned
void AEnemyCube::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// For testing: slowly spin the cube
	AddActorLocalRotation(FRotator(0.f, 50.f * DeltaTime, 0.f));
}

void AEnemyCube::OnHit(UPrimitiveComponent*, AActor* OtherActor,
	UPrimitiveComponent*, FVector, const FHitResult&)
{
	UE_LOG(LogTemp, Log, TEXT("EnemyCube hit %s"), *GetNameSafe(OtherActor));

	/* {
		if (OtherActor && OtherActor != GetOwner())
		{
			Destroy(); //destroy the cube on collision with other object
		}
	}*/
}

void AEnemyCube::OnOverlap(UPrimitiveComponent*, AActor* OtherActor,
	UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	UE_LOG(LogTemp, Log, TEXT("EnemyCube overlapped %s"), *GetNameSafe(OtherActor));
}




