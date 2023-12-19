// Fill out your copyright notice in the Description page of Project Settings.


#include "ReflectedAudioComponent.h"

#include "DMSettings.h"
#include "ReflectionComponent.h"
#include "ReflectionSystemConstants.h"
#include "Sound/AudioBus.h"
#include "Sound/SoundSourceBus.h"
#include "SourceEffects/SourceEffectEQ.h"
#include "SourceEffects/SourceEffectSimpleDelay.h"


// Sets default values for this component's properties
UReflectedAudioComponent::UReflectedAudioComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UReflectedAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	//Set this audio component output to an audio bus
	AudioBus = NewObject<UAudioBus>(this);
	SetAudioBusSendPostEffect(AudioBus, 1.0f);

	USoundSourceBus* SoundSourceBus = NewObject<USoundSourceBus>(this);
	SoundSourceBus->AudioBus = AudioBus;


	ReflectionDirections.Reserve(MaxReflections);
	//ReflectionPositions.Reserve(MaxReflections);
	ReflectionAudioComponents.Reserve(MaxReflections);

	const float Offset = 2.f / MaxReflections;
	const float Increment = PI * (3.f - sqrtf(5.f));

	USceneComponent* ReflectionsRoot = NewObject<USceneComponent>(this);
	ReflectionsRoot->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	ReflectionsRoot->RegisterComponent();
	ReflectionsRoot->SetRelativeLocation(FVector::ZeroVector);
	ReflectionsRoot->SetUsingAbsoluteRotation(true);

	for (int i = 0; i < MaxReflections; ++i)
	{
		const float y = ((i * Offset) - 1) + (Offset / 2);
		const float r = sqrtf(1.f - powf(y, 2.f));

		const float phi = ((i + 1) % MaxReflections) * Increment;

		float x = cosf(phi) * r;
		float z = sinf(phi) * r;

		FVector rayDirection = FVector(x, y, z*(1-HorizontalRaycastSkewing));//z*0.5f to make the reflections more horizontal
		
		ReflectionDirections.Emplace(rayDirection);

		UReflectionComponent* NewReflectionComponent = NewObject<UReflectionComponent>(this);
		NewReflectionComponent->SetupReflectionComponent(SoundSourceBus, SubReflections);
		NewReflectionComponent->AttachToComponent(ReflectionsRoot, FAttachmentTransformRules::KeepRelativeTransform);
		NewReflectionComponent->RegisterComponent();
		ReflectionAudioComponents.Emplace(NewReflectionComponent);
		//ReflectionPositions.Emplace(FVector::ZeroVector);
	}
}

// Called every frame
void UReflectedAudioComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bReflectionsEnabled == false)
	{
		if (areReflectionsEnabled)
		{
			for (int i = 0; i < MaxReflections; ++i)
			{
				ReflectionAudioComponents[i]->VolumeMultiplier = 0.0f;
			}
			areReflectionsEnabled = false;
		}
		return;
	}
	else
	{
		if (!areReflectionsEnabled)
		{
			currentReflectionIndex = 0;
			areReflectionsEnabled = true;
		}
	}
	// ...
	
	const FTransform OriginTransform = GetComponentTransform();
	//const FVector OriginLocation = GetComponentLocation();

	if (currentReflectionIndex < MaxReflections)
	{
		UE_LOG(LogTemp, Warning, TEXT("Recalculate reflection number %d"), currentReflectionIndex);

		UReflectionComponent* ReflectionAudioComponent = ReflectionAudioComponents[currentReflectionIndex];
		
		if(ReflectionAudioComponent->UpdateReflections(OriginTransform, ReflectionDirections[currentReflectionIndex], MaxReflectionDistance, currentSubReflectionIndex))
		{
			currentSubReflectionIndex = 0;
			currentReflectionIndex++;
		}
		else
		{
			currentSubReflectionIndex++;
		}
		
	}
	else if (needsToRecalculateReflections)
	{
		needsToRecalculateReflections = false;
		currentReflectionIndex = 0;
		currentSubReflectionIndex = 0;
	}
}

void UReflectedAudioComponent::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	Super::OnUpdateTransform(UpdateTransformFlags, Teleport);

	const FVector CurrentLocation = GetComponentLocation();
	if (FVector::DistSquared(CurrentLocation, PreviousLocation) > FMath::Square(ReflectionMovementThreshold))
	{
		PreviousLocation = CurrentLocation;
		needsToRecalculateReflections = true;
	}
}
