// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main_Character.generated.h"

UCLASS()
class SHOOTING_GAME_API AMain_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain_Character();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//foward or backward input
	void Move_Foward_or_Back(float value);

	//left or right input
	void Move_Left_or_Right(float value);

	//turning through input
	void TurnHorAtRate(float rate);
	
	//look up or down through input
	void LookVerAtRate(float rate);

	//fire the weapon
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& mSocketLocation, FVector& noBeamLocation);

	//use for set bAiming to true or false
	void AimingButtonPress();
	void AimingButtonRelease();

	//use for fire button
	void FireButtonPress();
	void FireButtonRelease();

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
	//cameraT is the camera behind the character
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraB;

	// camera that follow the character
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	//base turn tate, may affect by other scalling
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseHorRate;

	//base look up or down rate,may affect by other scalling
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseVerRate;

	//Rate when not aiming, or use for not aiming sensitivity
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipHorRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipVerRate;

	//Rate when aiming, or use for aiming sensitivity
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingHorRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingVerRate;

	//Rate when not aiming, or use for not aiming sensitivity(for mouse)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0" , ClampMax = "1.0", UIMin = "0.0" , UIMax = "1.0"))
	float mouseHipHorRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float mouseHipVerRate;

	////Rate when aiming, or use for aiming sensitivity(for mouse)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float mouseAimingHorRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float mouseAimingVerRate;

	//attack sound
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	//attack_effect
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* weapon_flash;

	//montage that use to attack
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipAttackMontage;

	//bullet impact
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactPar;

	//bullet smoke trail
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamPar;

	//aiming when true, not when false
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	//default camera when no aiming
	float CameraDefaultFOV;

	//camera when aiming
	float CameraZoomFOV;

	//current camera frame
	float CameraCurrentFOV;
	
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomInterSpeed;

	//for zooming camera
	void CAmeraIntepZoom(float DeltaTime);

	//for aiming or not aiming sensitivity
	void setSensitivity();

	//use for rotate controller by the mouse
	void mVer(float value);
	void mHor(float value);

	//fire buttom pressed
	bool bFireButtonPress;

	//fire when true, False when waiting for timer
	bool bFireOrder;

	//rate of auto fire
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float AutomaticFireRate;

	//timer between gunshots
	FTimerHandle AutoFireTimer;

public:
	// return cameraB subobject
	FORCEINLINE USpringArmComponent* GetCameraB() const { 
		return CameraB; 
	}

	//return followcamera
	FORCEINLINE UCameraComponent* GetFollowCamera() const {
		return FollowCamera;
	}

	FORCEINLINE bool GetAiming() const {
		return bAiming;
	}
};
