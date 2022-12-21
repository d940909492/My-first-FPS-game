// Fill out your copyright notice in the Description page of Project Settings.

#include "main_AnimInstance.h"
#include "Main_Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void Umain_AnimInstance::UpdateAnimaationProperties(float DeltaTime)
{
	if (Main_Character == nullptr) {
		Main_Character = Cast<AMain_Character>(TryGetPawnOwner());
	}
	if (Main_Character) {
		FVector Velocity{ Main_Character -> GetVelocity() };
		Velocity.Z = 0;
		speed = Velocity.Size();

		bIsInAir = Main_Character->GetCharacterMovement()->IsFalling();
	
		if (Main_Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f) {
			bIsAccelerating = true;
		}
		else {
			bIsAccelerating = false;
		}

		FRotator AimRotation = Main_Character->GetBaseAimRotation();

		FRotator MovementRotaion = UKismetMathLibrary::MakeRotFromX(Main_Character->GetVelocity());

		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotaion, AimRotation).Yaw;

		if (Main_Character->GetVelocity().Size() > 0.f) {
			StopMovementOffsetYaw = MovementOffsetYaw;
		}

		/*
		FString RT_Message = FString::Printf(TEXT("Base Aim Rotaion: %f"), AimRotation.Yaw);
		FString Offset_Message = FString::Printf(TEXT("Movement Offset Yaw: %f"), MovementOffsetYaw);

		if (GEngine) {
			//GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, RT_Message);
			//GEngine->AddOnScreenDebugMessage(2, 0.f, FColor::White, Offset_Message);
		}
		*/

		bAiming = Main_Character->GetAiming();
	}
}

void Umain_AnimInstance::NativeInitializeAnimation()
{
	Main_Character = Cast<AMain_Character>(TryGetPawnOwner());
}

