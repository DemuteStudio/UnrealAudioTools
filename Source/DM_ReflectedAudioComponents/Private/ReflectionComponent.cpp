// Fill out your copyright notice in the Description page of Project Settings.


#include "ReflectionComponent.h"

#include "DMSettings.h"
#include "Sound/SoundSourceBus.h"
#include "SourceEffects/SourceEffectEQ.h"
#include "SourceEffects/SourceEffectSimpleDelay.h"
#include "DrawDebugHelpers.h"
#include "Engine/HitResult.h"
#include "DSP/Filter.h"

static TAutoConsoleVariable<bool> CVarVisualizeReflections(
	TEXT("dm.VisualizeReflections"),
	false,
	TEXT("Visualize reflections for all ReflectedAudioComponents in the level."),
	ECVF_Default
);

UReflectionComponent::UReflectionComponent(): ReflectionMaterial(EReflectionMaterials::Default), SubReflectionComponent(nullptr), AudioBus(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UReflectionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const bool bVisualizeReflections = CVarVisualizeReflections.GetValueOnGameThread();

	if (!bVisualizeReflections) return;

	if (VolumeMultiplier == 0.0f) return; //Don't draw debug if the reflection is muted

	FColor DebugColor;
	switch (SubReflections)
	{
	default:
	case 0:
		DebugColor = FColor::Green;
		break;
	case 1:
		DebugColor = FColor::Blue;
		break;
	case 2:
		DebugColor = FColor::Cyan;
		break;
	case 3:
		DebugColor = FColor::Yellow;
		break;
	case 4:
		DebugColor = FColor::Magenta;
		break;
	}

	//Draw a sphere at the reflection position
	DrawDebugSphere(GetWorld(), GetComponentLocation(), 10.0f, 8, DebugColor, false, 0.0f, 0, 1.0f);

	//Draw text with the reflection material
	FString DebugString = "Material: ";
	switch (ReflectionMaterial)
	{
	case EReflectionMaterials::Default:
		DebugString += "Default";
		break;
	case EReflectionMaterials::UnglazedBrick:
		DebugString += "Unglazed Brick";
		break;
	case EReflectionMaterials::RoughConcrete:
		DebugString += "Rough Concrete";
		break;
	case EReflectionMaterials::Marble:
		DebugString += "Marble";
		break;
	case EReflectionMaterials::Sand:
		DebugString += "Sand";
		break;
	case EReflectionMaterials::WoodBeams:
		DebugString += "Wood Beams";
		break;
	case EReflectionMaterials::Snow:
		DebugString += "Snow";
		break;
	default:
		DebugString += "Material not found";
		break;
	}

	DebugString += "\nVolume Multiplier: ";
	DebugString += FString::SanitizeFloat(VolumeMultiplier);

	USourceEffectSimpleDelayPreset* DelayPreset = Cast<USourceEffectSimpleDelayPreset>(
		SourceEffectChain->Chain[0].Preset);

	if (DelayPreset)
	{
		DebugString += "\nDelay time: ";
		DebugString += FString::SanitizeFloat(DelayPreset->Settings.DelayAmount);
	}

	USourceEffectEQPreset* EQPreset = Cast<USourceEffectEQPreset>(
		SourceEffectChain->Chain[1].Preset);

	if (EQPreset)
	{
		DebugString += "\nEQ Values : ";
		for (FSourceEffectEQBand EQBand : EQPreset->Settings.EQBands)
		{
			DebugString += "\n";
			DebugString += FString::SanitizeFloat(EQBand.Frequency);
			DebugString += " : ";
			DebugString += FString::SanitizeFloat(EQBand.GainDb);
		}
	}


	DrawDebugString(GetWorld(), GetComponentLocation(), DebugString, nullptr, DebugColor, 0.0f, false, 1.0f);
}

void UReflectionComponent::SetupReflectionComponent(USoundSourceBus* InSoundSourceBus, int InSubReflections)
{
	SetSound(InSoundSourceBus);
	SourceEffectChain = NewObject<USoundEffectSourcePresetChain>(this);
	FSourceEffectChainEntry& delayEffectEntry = SourceEffectChain->Chain.AddDefaulted_GetRef();
	delayEffectEntry.Preset = NewObject<USourceEffectSimpleDelayPreset>(this);
	FSourceEffectChainEntry& eqEffectEntry = SourceEffectChain->Chain.AddDefaulted_GetRef();
	eqEffectEntry.Preset = NewObject<USourceEffectEQPreset>(this);
	SubReflections = InSubReflections;

	
	//Set this audio component output to an audio bus
	AudioBus = NewObject<UAudioBus>(this);
	SetAudioBusSendPostEffect(AudioBus, 1.0f);

	USoundSourceBus* SoundSourceBus = NewObject<USoundSourceBus>(this);
	SoundSourceBus->AudioBus = AudioBus;

	if (InSubReflections > 0)
	{
		UReflectionComponent* NewReflectionComponent = NewObject<UReflectionComponent>(this);
		NewReflectionComponent->SetupReflectionComponent(SoundSourceBus, InSubReflections - 1);
		NewReflectionComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
		NewReflectionComponent->RegisterComponent();
		SubReflectionComponent = NewReflectionComponent;
	}
}

bool UReflectionComponent::UpdateReflections(FTransform OriginTransform, FVector Direction, float MaxDistance,
                                             int subReflectionIndex)
{
	if (subReflectionIndex <= 0) // Calculate this reflection
	{
		//Raycast
		FHitResult HitResult;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(GetOwner());
		CollisionQueryParams.bTraceComplex = true;
		CollisionQueryParams.bReturnPhysicalMaterial = true;
		const FVector End = OriginTransform.GetLocation() + Direction * MaxDistance;
		GetWorld()->LineTraceSingleByChannel(HitResult, OriginTransform.GetLocation(), End, ECC_Visibility,
		                                     CollisionQueryParams);

		//Debug ray
		if (CVarVisualizeReflections.GetValueOnGameThread())
			DrawDebugLine(GetWorld(), OriginTransform.GetLocation(), End,
			              HitResult.bBlockingHit ? FColor::Green : FColor::Red,
			              false,
			              1.0f, 0, 1.0f);

		//Move reflection audio component to hit location
		if (HitResult.bBlockingHit)
		{
			SetWorldLocation(HitResult.Location);

			//Play audio and set audio parameters
			Play();
			FReflectionMaterial HitReflectionMaterial = UDMSettings::GetReflectionMaterial(
				HitResult.PhysMaterial.Get());

			UpdateAudioParameters(OriginTransform, HitResult.Location, HitReflectionMaterial);

			//If there are sub reflections, return false to calculate them on next frame and store the normal
			if (SubReflectionComponent != nullptr)
			{
				ReflectionNormal = HitResult.Normal;
				return false;
			}

			return true; //Done calculating reflections
		}

		//Nothing was hit so mute the reflection and return true to go to the next reflection
		MuteReflection();

		return true; //Done calculating reflections
	}
	else
	{
		//We already calculated this reflection, so we just need to update the sub reflection
		if (SubReflectionComponent != nullptr)
		{
			float RemainingDistance = MaxDistance -
				FVector::Dist(OriginTransform.GetLocation(), GetComponentLocation());
			return SubReflectionComponent->UpdateReflections(GetComponentTransform(), ReflectionNormal,
			                                                 RemainingDistance, subReflectionIndex - 1);
		}
		else
		{
			//No more sub reflections to calculate
			return true; // Done calculating reflections
		}
	}
}

void UReflectionComponent::MuteReflection()
{
	Stop();
	VolumeMultiplier = 0.0f;

	if (SubReflectionComponent != nullptr)
	{
		SubReflectionComponent->MuteReflection();
	}
}

void UReflectionComponent::UpdateAudioParameters(FTransform OriginTransform, FVector Location,
                                                 FReflectionMaterial HitMaterial)
{
	const FSoundAttenuationSettings* SoundAttenuationSettings = GetAttenuationSettingsToApply();
	if (SoundAttenuationSettings != nullptr)
	{
		float AttenuationVolumeMultiplier = SoundAttenuationSettings->Evaluate(
			OriginTransform, Location);
		VolumeMultiplier = AttenuationVolumeMultiplier;
	}
	else
	{
		VolumeMultiplier = 1.0f;
	}
	bOverrideAttenuation = true;
	const FSoundAttenuationSettings* attenuationToApply = GetAttenuationSettingsToApply();
	if (attenuationToApply != nullptr)
		AttenuationOverrides = *attenuationToApply;

	if (SourceEffectChain != nullptr && SourceEffectChain->Chain.Num() > 0)
	{
		USourceEffectSimpleDelayPreset* DelayPreset = Cast<USourceEffectSimpleDelayPreset>(
			SourceEffectChain->Chain[0].Preset);
		if (DelayPreset != nullptr)
		{
			DelayPreset->Settings.bDelayBasedOnDistance = true;
			DelayPreset->Settings.DelayAmount = FVector::Dist(OriginTransform.GetLocation(), Location)
				/
				34000.0f;
			UE_LOG(LogTemp, Warning, TEXT("Delay value %f"), DelayPreset->Settings.DelayAmount);
		}
		USourceEffectEQPreset* EQPreset = Cast<USourceEffectEQPreset>(
			SourceEffectChain->Chain[1].Preset);
		if (EQPreset != nullptr)
		{
			if (ReflectionMaterial != HitMaterial.ReflectionMaterial)
			{
				EQPreset->Settings.EQBands.Empty();
				for (int i = 0; i < HitMaterial.ReflectionCoefficients.Num(); ++i)
				{
					FSourceEffectEQBand& BandSettings = EQPreset->Settings.EQBands.AddDefaulted_GetRef();
					BandSettings.bEnabled = true;
					BandSettings.Frequency = 125.0f * powf(2.0f, (i - 1));
					BandSettings.GainDb = UReflectionSystemConstants::ReflectionCoefficientToDbReduction(
						HitMaterial.ReflectionCoefficients[i]);
					BandSettings.Bandwidth = 1.0f;
				}

				ReflectionMaterial = HitMaterial.ReflectionMaterial;
			}
		}
	}
}
