// Fill out your copyright notice in the Description page of Project Settings.


#include "GroupedAudioSourcesSubsystem.h"

UGroupedAudioSourcesSubsystem::UGroupedAudioSourcesSubsystem()
{
}

UGroupedAudioSourcesSubsystem::~UGroupedAudioSourcesSubsystem()
{
	SubsystemGroupsManager.Shutdown();
	SubsystemGroupsManager.Flush();
}

bool UGroupedAudioSourcesSubsystem::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	return Super::DoesSupportWorldType(WorldType) || WorldType == EWorldType::EditorPreview;
}

UGroupedAudioSourcesSubsystem* UGroupedAudioSourcesSubsystem::Get(UWorld* World)
{
	if(World)
	{
		return World->GetSubsystem<UGroupedAudioSourcesSubsystem>();
	}

	return nullptr;
}

UAudioSourcesGroupHandle* UGroupedAudioSourcesSubsystem::RegisterNewAudioSource(const UObject* WorldContextObject,
	FName GroupName, USceneComponent* AudioComponent, FAudioSourcesGroupSettings InSettings, bool
	bOverrideSettingsIfGroupExists)
{
	if(GroupName.IsNone())
	{
		return nullptr;
	}

	FGroupedAudioSourcesManager* GroupsManager = GetManagerForClock(WorldContextObject, GroupName);

	if(!GroupsManager)
	{
		return nullptr;
	}

	GroupsManager->RegisterNewAudioSource(GroupName, AudioComponent, InSettings, bOverrideSettingsIfGroupExists);

	return nullptr;

}

FGroupedAudioSourcesManager* UGroupedAudioSourcesSubsystem::GetManagerForClock(const UObject* WorldContextObject,
	FName ExistingGroupName)
{
	return &SubsystemGroupsManager;
}


void UGroupedAudioSourcesSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SubsystemGroupsManager.Tick(DeltaTime);
}

bool UGroupedAudioSourcesSubsystem::IsTickable() const
{
	const bool bIsManagingGroups = SubsystemGroupsManager.GetNumGroups() > 0;

	return bIsManagingGroups;
}

TStatId UGroupedAudioSourcesSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UGroupedAudioSourcesSubsystem, STATGROUP_Tickables);
}
