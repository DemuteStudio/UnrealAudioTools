// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GroupedAudioSourcesSubsystem.h"
#include "UObject/NoExportTypes.h"
#include "AudioSourcesGroupHandle.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, Transient, ClassGroup = GroupedAudioSources, meta = (BlueprintSpawnableComponent))
class DM_GROUPEDAUDIOSOURCES_API UAudioSourcesGroupHandle : public UObject
{
	GENERATED_BODY()

public:
	UAudioSourcesGroupHandle* Init(UWorld* InWorldPtr);

private:
	UPROPERTY(Transient)
	UWorld* WorldPtr{nullptr};

	UPROPERTY(Transient)
	UGroupedAudioSourcesSubsystem* GroupedAudioSourcesSubsystem;
};
