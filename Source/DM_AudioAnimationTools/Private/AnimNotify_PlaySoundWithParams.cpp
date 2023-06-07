// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_PlaySoundWithParams.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Animation/AnimSequenceBase.h"

void UAnimNotify_PlaySoundWithParams::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// Don't call super to avoid call back in to blueprints
	if (Sound && MeshComp)
	{
		if (Sound->IsLooping())
		{
			UE_LOG(LogAudio, Warning, TEXT("PlaySound notify: Anim %s tried to spawn infinitely looping sound asset %s. Spawning suppressed."), *GetNameSafe(Animation), *GetNameSafe(Sound));
			return;
		}

#if WITH_EDITORONLY_DATA
		UAudioComponent* spawnedComponent;
		UWorld* World = MeshComp->GetWorld();
		if (bPreviewIgnoreAttenuation && World && World->WorldType == EWorldType::EditorPreview)
		{
			spawnedComponent = UGameplayStatics::SpawnSound2D(World, Sound, VolumeMultiplier, PitchMultiplier);
		}
		else
#endif
		{
			if (bFollow)
			{
				spawnedComponent = UGameplayStatics::SpawnSoundAttached(Sound, MeshComp, AttachName, FVector(ForceInit), EAttachLocation::SnapToTarget, false, VolumeMultiplier, PitchMultiplier);
			}
			else
			{
				spawnedComponent = UGameplayStatics::SpawnSoundAtLocation(MeshComp->GetWorld(), Sound, MeshComp->GetComponentLocation(), MeshComp->GetComponentRotation(), VolumeMultiplier, PitchMultiplier);
			}
		}
		spawnedComponent->InstanceParameters = Parameters;
	}
}

bool UAnimNotify_PlaySoundWithParams::SetSound_Implementation(UObject* SoundObject)
{
	USoundBase* CastedSound = Cast<USoundBase>(SoundObject);
	if(CastedSound)
	{
		Sound = CastedSound;
		return true;
	}
	return false;
}

bool UAnimNotify_PlaySoundWithParams::SetFollow_Implementation(const bool InterfaceFollow)
{
	bFollow = InterfaceFollow;
	return true;
}

bool UAnimNotify_PlaySoundWithParams::SetParameters_Implementation(const TArray<FAudioAnimNotifyParameter>& ParametersObject)
{
	Parameters.Empty();
	bool result = false;
	for (const FAudioAnimNotifyParameter ParameterObject : ParametersObject)
	{
		FAudioComponentParam CastedParameter;
		CastedParameter.ParamName = ParameterObject.ParameterName;
		CastedParameter.FloatParam = ParameterObject.ParameterValue;
		Parameters.Add(CastedParameter);
		result = true;
	}
	return result;
}

bool UAnimNotify_PlaySoundWithParams::SetAttachName_Implementation(FName InterfaceAttachName)
{
	AttachName = InterfaceAttachName;
	return true;
}
