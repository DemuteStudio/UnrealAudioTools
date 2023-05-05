// Fill out your copyright notice in the Description page of Project Settings.


#include "AudioSourcesGroupHandle.h"

#include "GroupedAudioSourcesSubsystem.h"

UAudioSourcesGroupHandle* UAudioSourcesGroupHandle::Init(UWorld* InWorldPtr)
{
	checkSlow(InWorldPtr);

	WorldPtr = InWorldPtr;

	GroupedAudioSourcesSubsystem = UGroupedAudioSourcesSubsystem::Get(WorldPtr);

	GroupedAudioSourcesSubsystem->SubscribeToGroupedAudioSourcesTick(this);

	return this;
}

