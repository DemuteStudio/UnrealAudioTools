// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AudioAnimNotifyInterface.h"
#include "Animation/AnimNotifies/AnimNotify_PlaySound.h"
#include "Components/AudioComponent.h"
#include "AnimNotify_PlaySoundWithParams.generated.h"

/**
 * 
 */
UCLASS()
class DM_AUDIOANIMATIONTOOLS_API UAnimNotify_PlaySoundWithParams : public UAnimNotify_PlaySound, public IAudioAnimNotifyInterface
{
	GENERATED_BODY()
public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimNotify", meta=(ExposeOnSpawn = true))
	TArray<FAudioComponentParam> Parameters;

	virtual bool SetSound_Implementation(UObject* SoundObject) override;

	virtual bool SetFollow_Implementation(const bool InterfaceFollow) override;

	virtual bool SetParameters_Implementation(const TArray<FAudioAnimNotifyParameter>& ParametersObject) override;

	virtual bool SetAttachName_Implementation(FName InterfaceAttachName) override;
};
