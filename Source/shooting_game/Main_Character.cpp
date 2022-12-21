// Fill out your copyright notice in the Description page of Project Settings.

#include "Main_Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "items.h"
#include "Components/WidgetComponent.h"


// Sets default values
AMain_Character::AMain_Character(): BaseHorRate(45.f) , BaseVerRate(45.f) , 
bAiming(false) , CameraDefaultFOV(0.f), CameraZoomFOV(50.f) , CameraCurrentFOV(0.f),ZoomInterSpeed(20.f),
HipHorRate(90.f), HipVerRate(90.f), AimingHorRate(60.f), AimingVerRate(60.f),
mouseHipHorRate(1.0f),mouseHipVerRate(1.0f),mouseAimingHorRate(0.8f),mouseAimingVerRate(0.8f),
AutomaticFireRate(0.5f), bFireOrder(true), bFireButtonPress(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//create a cameraB, it pull in toward the character if there is collision
	CameraB = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraB"));
	CameraB ->SetupAttachment(RootComponent);
	//the camera fllow at this distance behind the character
	CameraB->TargetArmLength = 200.f;
	//rotate the arm base on the controller
	CameraB->bUsePawnControlRotation = true;
	//
	CameraB->SocketOffset = FVector(0.f, 50.f, 50.f);


	//use to create the FollowCamera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//attach camera to the end of boom
	FollowCamera->SetupAttachment(CameraB, USpringArmComponent::SocketName);
	//camera do not need to rotate
	FollowCamera->bUsePawnControlRotation = false;

	//dont rotate when the controller rotates, let the controller only affect the camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	//character movement setting
	GetCharacterMovement()->bOrientRotationToMovement = false; //direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AMain_Character::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BeginPlay() funtion being called"));

	if (FollowCamera) {
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
	
}

void AMain_Character::Move_Foward_or_Back(float value)
{
	if ((Controller != nullptr) && (value != 0.0f)) {
		const FRotator Rotation{
			Controller->GetControlRotation()
		};
		const FRotator YawRotaion{
			0, Rotation.Yaw, 0
		};
		const FVector Direction{
			FRotationMatrix{YawRotaion}.GetUnitAxis(EAxis::X)
		};

		AddMovementInput(Direction, value);
	}
}

void AMain_Character::Move_Left_or_Right(float value)
{
	if ((Controller != nullptr) && (value != 0.0f)) {
		const FRotator Rotation{
			Controller->GetControlRotation()
		};
		const FRotator YawRotaion{
			0, Rotation.Yaw, 0
		};
		const FVector Direction{
			FRotationMatrix{YawRotaion}.GetUnitAxis(EAxis::Y)
		};

		AddMovementInput(Direction, value);
	}
}

void AMain_Character::TurnHorAtRate(float rate)
{
	// this would calculate delta for this frame from rate information
	AddControllerYawInput(rate * BaseHorRate * GetWorld()->GetDeltaSeconds());
}

void AMain_Character::LookVerAtRate(float rate)
{
	AddControllerPitchInput(rate * BaseVerRate * GetWorld()->GetDeltaSeconds());
}

void AMain_Character::FireWeapon()
{
	if (FireSound) {
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName("mFlash");

	if (WeaponSocket) {
		const FTransform SocketTranform = WeaponSocket->GetSocketTransform(GetMesh());
		if (weapon_flash) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), weapon_flash, SocketTranform);
		}

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(SocketTranform.GetLocation(), BeamEnd);

		if (bBeamEnd) {
			if (ImpactPar) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactPar, BeamEnd);
			}

			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamPar, SocketTranform);
			if (Beam) {
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
		}
		}


	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipAttackMontage) {
		AnimInstance->Montage_Play(HipAttackMontage);
		AnimInstance->Montage_JumpToSection(FName("start_attack"));
	}
}

//function use to optimize beam codes
bool AMain_Character::GetBeamEndLocation(const FVector& mSocketLocation, FVector& noBeamLocation)
{
	//get size of the viewport
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	//get crosshair's screen space location 
	FVector2D CrosshairLocation((ViewportSize.X / 2.f), (ViewportSize.Y / 2.f));
	//fix beam Y location
	CrosshairLocation.Y += 27.f;
	FVector CrosshairWorldPosition;
	FVector CrosshairworldDirection;

	//Get World postion and direction
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairworldDirection);

	if (bScreenToWorld) { //checking deprojection
		FHitResult ScreenTraceHit;
		const FVector start{ CrosshairWorldPosition };
		const FVector end{ CrosshairWorldPosition + CrosshairworldDirection * 50'000.f };

		//set  beam end point to line trace end point
		noBeamLocation = end;

		//trace outward from crosshair world location
		GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, start, end, ECollisionChannel::ECC_Visibility);
		if (ScreenTraceHit.bBlockingHit) { // checking trace hit

			// beam end point is in trace location
			noBeamLocation = ScreenTraceHit.Location;
		}

		//check if beam hit some object to stop , not just pass it
		FHitResult WeaponTraceHit;
		const FVector WeaponTraceStart{ mSocketLocation};
		const FVector WeaponTraceEnd{ noBeamLocation };
		GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);
		if (WeaponTraceHit.bBlockingHit) {
			noBeamLocation = WeaponTraceHit.Location;
		}
		return true;
	}
	return false;
}

void AMain_Character::AimingButtonPress()
{
	bAiming = true;
}

void AMain_Character::AimingButtonRelease()
{
	bAiming = false;
}

void AMain_Character::FireButtonPress()
{
	bFireButtonPress = true;
	StartFireTimer();
}

void AMain_Character::FireButtonRelease()
{
	bFireButtonPress = false;
}

void AMain_Character::StartFireTimer()
{
	if (bFireOrder) {
		FireWeapon();
		bFireOrder = false;
		GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AMain_Character::AutoFireReset, AutomaticFireRate);
	}
}

void AMain_Character::AutoFireReset()
{
	bFireOrder = true;
	if (bFireButtonPress) {
		StartFireTimer();
	}
}

bool AMain_Character::TraceLineUnderCrosshair(FHitResult& OutHitResult)
{
	//get viewport size
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	//get crosshair's screen space location 
	FVector2D CrosshairLocation((ViewportSize.X / 2.f), (ViewportSize.Y / 2.f));
	//fix beam Y location
	CrosshairLocation.Y += 27.f;
	FVector CrosshairWorldPosition;
	FVector CrosshairworldDirection;

	//Get World postion and direction
	bool bScreenToWorld = 
	UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), 
	CrosshairLocation, CrosshairWorldPosition, CrosshairworldDirection);

	if (bScreenToWorld) {
		//Trace from crosshair world location outward
		FVector Start{ CrosshairWorldPosition};
		FVector End{ Start + CrosshairworldDirection * 50'000.f};
		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		//checking is there is a blocking hit or not
		if (OutHitResult.bBlockingHit) {
			return true;
		}
	}

	return false;
}

// Called every frame
void AMain_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//setting current camera FOV
	CAmeraIntepZoom(DeltaTime);

	//setting Sensitivity
	setSensitivity();

	FHitResult LineTraceResult;
	TraceLineUnderCrosshair(LineTraceResult);
	if (LineTraceResult.bBlockingHit) {
		Aitems* HitItem = Cast<Aitems>(LineTraceResult.GetActor());
		if (HitItem && HitItem -> GetPickupWidget()) {
			//show item pick up widget
			HitItem->GetPickupWidget()->SetVisibility(true);
		}
	}
}


void AMain_Character::CAmeraIntepZoom(float DeltaTime)
{
	if (bAiming) {
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomFOV, DeltaTime, ZoomInterSpeed);
	}
	else {
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterSpeed);
	}
	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}

void AMain_Character::setSensitivity()
{
	if (bAiming) {
		BaseHorRate = AimingHorRate;
		BaseVerRate = AimingVerRate;
	}
	else {
		BaseHorRate = HipHorRate;
		BaseVerRate = HipVerRate;
	}
}

void AMain_Character::mHor(float value)
{
	float HorScaleFactor{};
	if (bAiming) {
		HorScaleFactor = mouseAimingHorRate;
	}
	else {
		HorScaleFactor = mouseHipHorRate;
	}
	AddControllerYawInput(value * HorScaleFactor);
}

void AMain_Character::mVer(float value)
{
	float VerScaleFactor{};
	if (bAiming) {
		VerScaleFactor = mouseAimingVerRate;
	}
	else {
		VerScaleFactor = mouseHipVerRate;
	}
	AddControllerPitchInput(value * VerScaleFactor);
}



// Called to bind functionality to input
void AMain_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("move_foward_or_back", this, &AMain_Character::Move_Foward_or_Back);
	PlayerInputComponent->BindAxis("move_left_or_right", this, &AMain_Character::Move_Left_or_Right);

	PlayerInputComponent->BindAxis("HorRate", this, &AMain_Character::TurnHorAtRate);
	PlayerInputComponent->BindAxis("VerRate", this, &AMain_Character::LookVerAtRate);

	PlayerInputComponent->BindAxis("HorM", this, &AMain_Character::mHor);
	PlayerInputComponent->BindAxis("VerM", this, &AMain_Character::mVer);

	PlayerInputComponent->BindAction("jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("fire_buttom", IE_Pressed, this, &AMain_Character::FireButtonPress);
	PlayerInputComponent->BindAction("fire_buttom", IE_Released, this, &AMain_Character::FireButtonRelease);

	PlayerInputComponent->BindAction("Aiming", IE_Pressed, this, &AMain_Character::AimingButtonPress);
	PlayerInputComponent->BindAction("Aiming", IE_Released, this, &AMain_Character::AimingButtonRelease);
}