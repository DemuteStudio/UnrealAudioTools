// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticMeshAudioEmitterBase.h"

#include "StaticMeshEmitterSourceComponent.h"

// Sets default values
AStaticMeshAudioEmitterBase::AStaticMeshAudioEmitterBase(const class FObjectInitializer& ObjectInitializer) : Super (ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SourceComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshEmitterSourceComponent>(this, TEXT("SourceComponent"));
}

// Called when the game starts or when spawned
void AStaticMeshAudioEmitterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStaticMeshAudioEmitterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStaticMeshAudioEmitterBase::SetNewReferencePosition(const FVector& NewPosition)
{
	SetActorLocation(PositionOffset? (GetActorLocation() - OriginalPosition)+NewPosition : NewPosition);
	OriginalPosition = NewPosition;
}

void AStaticMeshAudioEmitterBase::SetNewReferenceRotation(const FRotator& NewRotation)
{
	SetActorRotation(PositionOffset? (GetActorRotation()- OriginalRotation) + NewRotation : NewRotation );
	OriginalRotation = NewRotation;
}

