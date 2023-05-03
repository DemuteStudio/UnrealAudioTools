// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GroupedAudioSourcesUtilities.generated.h"

ENGINE_API DECLARE_LOG_CATEGORY_EXTERN(LogAudioGroups, Log, All);

USTRUCT(BlueprintType)
struct DM_GROUPEDAUDIOSOURCES_API FAudioSourcesGroupSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grouped Audio Sources Settings")
	float MaxAttenuationDistance;
	
};