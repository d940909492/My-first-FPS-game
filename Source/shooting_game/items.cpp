// Fill out your copyright notice in the Description page of Project Settings.


#include "items.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
Aitems::Aitems()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ITem mesh"));
	SetRootComponent(ItemMesh);

	CollisionBox = CreateAbstractDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Pick up Widget"));
	PickupWidget->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void Aitems::BeginPlay()
{
	Super::BeginPlay();
	PickupWidget-> SetVisibility(false);
}

// Called every frame
void Aitems::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

