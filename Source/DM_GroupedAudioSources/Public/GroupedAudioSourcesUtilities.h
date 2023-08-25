// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GroupedAudioSourcesUtilities.generated.h"

//forwards
class UGroupedAudioComponent;

ENGINE_API DECLARE_LOG_CATEGORY_EXTERN(LogAudioGroups, Log, All);

USTRUCT(BlueprintType)
struct DM_GROUPEDAUDIOSOURCES_API FAudioSourcesGroupSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grouped Audio Sources Settings")
	float MaxAttenuationDistance;
	
};

UCLASS()
class DM_GROUPEDAUDIOSOURCES_API UAudioCluster : public UObject
{

	GENERATED_BODY()

public:

	void ChangeClusterRoot(UGroupedAudioComponent* NewRootComponent, FVector NewLocation, float NewDistanceSquared);

	UPROPERTY()
	FVector ClusterLocation;
	
	UPROPERTY()
	float ClusterDistanceSquared;
	
	UPROPERTY()
	UGroupedAudioComponent* ClosestComponent;
};