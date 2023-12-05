// Fill out your copyright notice in the Description page of Project Settings.


#include "ReflectionComponent.h"

#include "SourceEffects/SourceEffectEQ.h"
#include "SourceEffects/SourceEffectSimpleDelay.h"


static TAutoConsoleVariable<bool> CVarVisualizeReflections(
	TEXT("dm.VisualizeReflections"),
	false,
	TEXT("Visualize reflections for all ReflectedAudioComponents in the level."),
	ECVF_Default
);

UReflectionComponent::UReflectionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UReflectionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bool bVisualizeReflections = CVarVisualizeReflections.GetValueOnGameThread();

	if(!bVisualizeReflections) return;

	//Draw a sphere at the reflection position
	DrawDebugSphere(GetWorld(), GetComponentLocation(), 10.0f, 8, FColor::Green, false, 0.0f, 0, 1.0f);

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

	if(DelayPreset)
	{
		DebugString += "\nDelay time: ";
		DebugString += FString::SanitizeFloat(DelayPreset->Settings.DelayAmount);
	}

	USourceEffectEQPreset* EQPreset = Cast<USourceEffectEQPreset>(
		SourceEffectChain->Chain[1].Preset);

	if(EQPreset)
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
	
	
	DrawDebugString(GetWorld(), GetComponentLocation(), DebugString, nullptr, FColor::Green, 0.0f, false, 1.0f);

	
	
}
