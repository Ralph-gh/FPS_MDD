// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FPSCharacter.h"
#include "HUD/DefenseHUD.h"   
#include "DrawDebugHelpers.h"
#include "Projectile/FPSProjectile.h"
#include "Engine/Engine.h"
#include <AFPSProjectGameModeBase.h>
// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*if (!FPSCameraComponent) {
		FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera")); // AddComponent in Unity
		FPSCameraComponent->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));
		FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));
		FPSCameraComponent->bUsePawnControlRotation = true;
	}

	if (!FPSMesh) {
		FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
		FPSMesh->SetupAttachment(FPSCameraComponent);
		FPSMesh->bCastDynamicShadow = false;
		FPSMesh->CastShadow = false;
	}

	GetMesh()->SetOwnerNoSee(true);
	*/
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("AFPSCharacter C++ BeginPlay"));
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//I like to move it move it
	PlayerInputComponent->BindAxis("MoveForward",this, &AFPSCharacter::MoveForward);

	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	//Look
	PlayerInputComponent->BindAxis("LookHorizontal", this, &AFPSCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookVertical", this, &AFPSCharacter::AddControllerPitchInput);
	//Jumpy Jumpy
	PlayerInputComponent->BindAction("Jump", IE_Pressed,this, &AFPSCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::EndJump);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::Fire);

	PlayerInputComponent->BindKey(EKeys::O, IE_Pressed, this, &AFPSCharacter::ToggleMenu);

	PlayerInputComponent->BindAction("ToggleDimension", IE_Pressed, this, &AFPSCharacter::ToggleDimensionPressed);

}

void AFPSCharacter::MoveForward(float value)
{
	//FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	//AddMovementInput(Direction, value);

	FVector Direction = GetActorForwardVector();
	AddMovementInput(Direction, value);
}

void AFPSCharacter::MoveRight(float value)
{
	FVector Direction = GetActorRightVector();
	AddMovementInput(Direction, value);
}

void AFPSCharacter::StartJump()
{
	bPressedJump = true;

}

void AFPSCharacter::EndJump()
{
	bPressedJump = false;
}

void AFPSCharacter::Fire()
{

	UWorld* World = GetWorld();
	if (!World) return;
	// Camera location & rotation
	FVector CamLoc;
	FRotator CamRot;
	GetActorEyesViewPoint(CamLoc, CamRot);

	

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, TEXT("FIRE pressed"));
	}

	const float TraceRange = 5000.f; // 50 meters
	const FVector Start = CamLoc;
	const FVector End = Start + (CamRot.Vector() * TraceRange);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(WeaponTrace), /*bTraceComplex=*/true, this);
	Params.bReturnPhysicalMaterial = false;

	FHitResult Hit;

	const bool bHit = World->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	// Draw debug line so you can see it in PIE
	const FColor LineColor = bHit ? FColor::Red : FColor::Green;
	DrawDebugLine(World, Start, bHit ? Hit.ImpactPoint : End, LineColor, false, 0.2f, 0, 0.2f);

	if (bHit && Hit.GetActor())
	{
		// Simple feedback in the log
		UE_LOG(LogTemp, Log, TEXT("Hit: %s at %s"),
			*Hit.GetActor()->GetName(),
			*Hit.ImpactPoint.ToString());

		// TODO: Apply damage later:
		// UGameplayStatics::ApplyPointDamage(Hit.GetActor(), 10.f, CamRot.Vector(), Hit, GetController(), this, UDamageType::StaticClass());
	}

	if (!ProjectileClass) return;

	const FVector SpawnLoc =
		CamLoc + CamRot.Vector() * MuzzleOffset.X +
		CamRot.RotateVector(FVector(0.f, MuzzleOffset.Y, MuzzleOffset.Z));

	const FRotator SpawnRot = CamRot;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	World->SpawnActor<AFPSProjectile>(ProjectileClass, SpawnLoc, SpawnRot, SpawnParams);
	/*if (!ProjectileClass) return;

	// Init relevant infomration for where the projectile will be
	FVector CameraLocation;
	FRotator CameraRotation;
	GetActorEyesViewPoint(CameraLocation, CameraRotation);

	MuzzleOffset.Set(100.0f, 0.0f, 0.0f);

	FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

	FRotator MuzzleRotation = CameraRotation;
	MuzzleRotation.Pitch += 10.0f;

	// Start of spawning the projectile
	UWorld* World = GetWorld();
	if (!World)  return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	// Unity Instantiate
	AFPSProjectile* Projectile = World->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
	if (!Projectile) return;

	// Launch spawned projectile in the camera rotation
	FVector LaunchDirection = MuzzleRotation.Vector();
	Projectile->FireInDirection(LaunchDirection);*/
}

void AFPSCharacter::ToggleMenu()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// Either way works — pick one:

		// A) Using the templated getter (needs the include above)
		if (ADefenseHUD* HUD = PC->GetHUD<ADefenseHUD>())
		{
			HUD->ToggleSettingsMenu();   // <-- call settings toggle (not main menu)
		}

	}
}
void AFPSCharacter::ToggleDimensionPressed()
{
	if (UWorld* W = GetWorld())
	{
		if (AFPSProjectGameModeBase* GM = W->GetAuthGameMode<AFPSProjectGameModeBase>())
		{
			GM->ToggleDimension();
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, TEXT("Toggle pressed"));
			}
		}
	}
}