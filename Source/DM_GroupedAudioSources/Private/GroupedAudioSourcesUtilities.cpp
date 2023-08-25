// Fill out your copyright notice in the Description page of Project Settings.


#include "GroupedAudioSourcesUtilities.h"

#include "DrawDebugHelpers.h"
#include "GroupedAudioComponent.h"

void UAudioCluster::ChangeClusterRoot(UGroupedAudioComponent* NewRootComponent, FVector NewLocation,
                                      float NewDistanceSquared)
{
	if(ClosestComponent)
	{
		ClosestComponent->turnOn = false;
	}
	NewRootComponent->turnOn = true;
	ClosestComponent = NewRootComponent;
	ClusterLocation = NewLocation;
	ClusterDistanceSquared = NewDistanceSquared;
}
