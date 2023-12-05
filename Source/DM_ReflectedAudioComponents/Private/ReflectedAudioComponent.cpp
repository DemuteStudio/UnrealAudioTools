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
	ReflectionPositions.Reserve(MaxReflections);
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

		UReflectionComponent* NewAudioComponent = NewObject<UReflectionComponent>(this);
		NewAudioComponent->AttachToComponent(ReflectionsRoot, FAttachmentTransformRules::KeepRelativeTransform);
		NewAudioComponent->RegisterComponent();
		NewAudioComponent->SetSound(SoundSourceBus);
		NewAudioComponent->SourceEffectChain = NewObject<USoundEffectSourcePresetChain>(NewAudioComponent);
		FSourceEffectChainEntry& delayEffectEntry = NewAudioComponent->SourceEffectChain->Chain.AddDefaulted_GetRef();
		delayEffectEntry.Preset = NewObject<USourceEffectSimpleDelayPreset>(NewAudioComponent);
		FSourceEffectChainEntry& eqEffectEntry = NewAudioComponent->SourceEffectChain->Chain.AddDefaulted_GetRef();
		eqEffectEntry.Preset = NewObject<USourceEffectEQPreset>(NewAudioComponent);
		ReflectionAudioComponents.Emplace(NewAudioComponent);
		ReflectionPositions.Emplace(FVector::ZeroVector);
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
	const FVector OriginLocation = GetComponentLocation();

	if (currentReflectionIndex < MaxReflections)
	{
		UE_LOG(LogTemp, Warning, TEXT("Recalculate reflection number %d"), currentReflectionIndex);

		UReflectionComponent* ReflectionAudioComponent = ReflectionAudioComponents[currentReflectionIndex];

		//Raycast
		FHitResult HitResult;
		const FVector End = OriginLocation + ReflectionDirections[currentReflectionIndex] * MaxReflectionDistance;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(GetOwner());
		CollisionQueryParams.bTraceComplex = true;
		CollisionQueryParams.bReturnPhysicalMaterial = true;
		GetWorld()->LineTraceSingleByChannel(HitResult, OriginLocation, End, ECC_Visibility, CollisionQueryParams);

		//Debug ray
		DrawDebugLine(GetWorld(), OriginLocation, End, HitResult.bBlockingHit ? FColor::Green : FColor::Red, false,
		              1.0f, 0, 1.0f);

		//Move reflection audio component to hit location
		if (HitResult.bBlockingHit)
		{
			ReflectionPositions[currentReflectionIndex] = HitResult.Location;

			ReflectionAudioComponent->SetWorldLocation(HitResult.Location);
			ReflectionAudioComponent->Play();
			const FSoundAttenuationSettings* SoundAttenuationSettings = GetAttenuationSettingsToApply();
			if (SoundAttenuationSettings != nullptr)
			{
				float AttenuationVolumeMultiplier = SoundAttenuationSettings->Evaluate(
					OriginTransform, HitResult.Location);
				ReflectionAudioComponent->VolumeMultiplier = AttenuationVolumeMultiplier;
			}
			else
			{
				ReflectionAudioComponent->VolumeMultiplier = 1.0f;
			}
			ReflectionAudioComponent->bOverrideAttenuation = true;
			const FSoundAttenuationSettings* attenuationToApply = GetAttenuationSettingsToApply();
			if(attenuationToApply != nullptr)
				ReflectionAudioComponent->AttenuationOverrides = *attenuationToApply;

			USoundEffectSourcePresetChain* ReflectionSourceEffectChain = ReflectionAudioComponents[
					currentReflectionIndex]->
				SourceEffectChain;
			if (ReflectionSourceEffectChain != nullptr && ReflectionSourceEffectChain->Chain.Num() > 0)
			{
				USourceEffectSimpleDelayPreset* DelayPreset = Cast<USourceEffectSimpleDelayPreset>(
					ReflectionSourceEffectChain->Chain[0].Preset);
				if (DelayPreset != nullptr)
				{
					DelayPreset->Settings.bDelayBasedOnDistance = true;
					DelayPreset->Settings.DelayAmount = FVector::Dist(OriginLocation, HitResult.Location) /
						34000.0f;
					UE_LOG(LogTemp, Warning, TEXT("Delay value %f"), DelayPreset->Settings.DelayAmount);
				}
				USourceEffectEQPreset* EQPreset = Cast<USourceEffectEQPreset>(
					ReflectionSourceEffectChain->Chain[1].Preset);
				if(EQPreset != nullptr)
				{
					FReflectionMaterial ReflectionMaterial = UDMSettings::GetReflectionMaterial(HitResult.PhysMaterial.Get());
					if(ReflectionAudioComponent->ReflectionMaterial != ReflectionMaterial.ReflectionMaterial)
					{
						EQPreset->Settings.EQBands.Empty();
						for (int i = 0; i < ReflectionMaterial.ReflectionCoefficients.Num(); ++i)
						{
							FSourceEffectEQBand& BandSettings = EQPreset->Settings.EQBands.AddDefaulted_GetRef();
							BandSettings.bEnabled = true;
							BandSettings.Frequency = 125.0f * powf(2.0f, (i-1));
							BandSettings.GainDb = UReflectionSystemConstants::ReflectionCoefficientToDbReduction(
								ReflectionMaterial.ReflectionCoefficients[i]);
							BandSettings.Bandwidth = 1.0f;
						}

						ReflectionAudioComponent->ReflectionMaterial = ReflectionMaterial.ReflectionMaterial;
					}
				}
			}
		}
		else
		{
			ReflectionPositions[currentReflectionIndex] = FVector::ZeroVector;

			ReflectionAudioComponent->Stop();
			ReflectionAudioComponent->VolumeMultiplier = 0.0f;
		}

		currentReflectionIndex++;
	}
	else if (needsToRecalculateReflections)
	{
		needsToRecalculateReflections = false;
		currentReflectionIndex = 0;
	}

	// //Draw audio component positions
	// for(int i = 0; i < ReflectionAudioComponents.Num(); ++i)
	// {
	// 	DrawDebugSphere(GetWorld(), ReflectionAudioComponents[i]->GetComponentLocation(), 10.0f, 8, FColor::Blue, false, 1.0f, 0, 1.0f);
	// }

	// for (int i = 0; i < MaxReflections; ++i)
	// {
	// 	if (ReflectionPositions[i] != FVector::ZeroVector)
	// 	{
	// 		FVector interpLocation = FMath::VInterpConstantTo(
	// 			ReflectionAudioComponents[i]->GetComponentLocation(),
	// 			ReflectionPositions[i],
	// 			DeltaTime,
	// 			ReflectionInterpolationSpeed);
	// 		ReflectionAudioComponents[i]->SetWorldLocation(interpLocation);
	// 		ReflectionAudioComponents[i]->Play();
	// 		float AttenuationVolumeMultiplier = GetAttenuationSettingsToApply()->Evaluate(
	// 			OriginTransform, interpLocation);
	// 		ReflectionAudioComponents[i]->VolumeMultiplier = AttenuationVolumeMultiplier;
	// 		ReflectionAudioComponents[i]->bOverrideAttenuation = true;
	// 		ReflectionAudioComponents[i]->AttenuationOverrides = *GetAttenuationSettingsToApply();
	// 	}
	// 	else
	// 	{
	// 		ReflectionAudioComponents[i]->Stop();
	// 		ReflectionAudioComponents[i]->VolumeMultiplier = 0.0f;
	// 	}
	// }
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
