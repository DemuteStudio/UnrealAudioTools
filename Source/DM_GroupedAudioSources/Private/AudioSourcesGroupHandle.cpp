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

int UAudioSourcesGroupHandle::GetNumberOfActiveSources(const UObject* WorldContextObject) const
{
	if (GroupedAudioSourcesSubsystem)
	{
		FGroupedAudioSourcesManager* GroupedSourcesManager = GroupedAudioSourcesSubsystem->GetManagerForGroup(WorldContextObject, GetGroupName());

		if (GroupedSourcesManager)
		{
			return GroupedSourcesManager->GetActiveSourcesCountForGroup(CurrentGroupId);
		}
	}

	return 0;
}

int UAudioSourcesGroupHandle::GetNumberOfActiveClusters(const UObject* WorldContextObject) const
{
	if (GroupedAudioSourcesSubsystem)
	{
		FGroupedAudioSourcesManager* GroupedSourcesManager = GroupedAudioSourcesSubsystem->GetManagerForGroup(WorldContextObject, GetGroupName());

		if (GroupedSourcesManager)
		{
			return GroupedSourcesManager->GetActiveClustersCountForGroup(CurrentGroupId);
		}
	}

	return 0;
}

UAudioSourcesGroupHandle* UAudioSourcesGroupHandle::SubscribeToGroup(const UObject* WorldContextObject, FName GroupName)
{
	// create ID
	CurrentGroupId = GroupName;

	FString TempId = WorldContextObject->GetFName().ToString();
	TempId.Append(CurrentGroupId.ToString());
	GroupHandleId = FName(*TempId);

	// TODO: subscribe to clock w/ ClockHandleId
	bConnectedToGroup = true;

	return this;
}

