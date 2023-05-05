// Fill out your copyright notice in the Description page of Project Settings.


#include "GroupedAudioSourcesManager.h"

FGroupedAudioSourcesManager::FGroupedAudioSourcesManager()
{
}

FGroupedAudioSourcesManager::~FGroupedAudioSourcesManager()
{
}

void FGroupedAudioSourcesManager::Tick(float DeltaTime)
{
	for (const TSharedPtr<FAudioSourcesGroup> ActiveGroup : ActiveGroups)
	{
		ActiveGroup->Tick(DeltaTime);
	}
}

void FGroupedAudioSourcesManager::Shutdown()
{
	ActiveGroups.Reset();
}

void FGroupedAudioSourcesManager::Flush()
{
	const int32 NumGroups = ActiveGroups.Num();

	for (int i = NumGroups-1; i >= 0; --i)
	{
		ActiveGroups.RemoveAtSwap(i);
	}
}

TSharedPtr<FAudioSourcesGroup> FGroupedAudioSourcesManager::RegisterNewAudioSource(const FName& InGroupName,
	USceneComponent* AudioComponent, const FAudioSourcesGroupSettings& InSettings, bool bOverrideSettingsIfGroupExists)
{
	//make a copy of the Settings
	FAudioSourcesGroupSettings NewSettings = InSettings;

	//See if this group already exists
	TSharedPtr<FAudioSourcesGroup> Group = FindGroup(InGroupName);

	if(Group)
	{
		if(bOverrideSettingsIfGroupExists && !Group->DoesMatchSettings(NewSettings))
		{
			UE_LOG(LogAudioGroups, Display, TEXT("Overriding settings on Audio Group : %s"), *Group->GetName().ToString());
			Group->ChangeGroupSettings(NewSettings);
		}
	}
	else
	{
		Group = ActiveGroups.Emplace_GetRef(MakeShared<FAudioSourcesGroup>(InGroupName, NewSettings, this));
	}

	Group->TrackedAudioComponents.AddUnique(AudioComponent);
	
	return Group;
}

void FGroupedAudioSourcesManager::UnregisterAudioSource(const FName& InName, USceneComponent* SceneComponent)
{
	const int32 NumGroups = ActiveGroups.Num();
	for (int32 i = NumGroups - 1; i>=0; --i)
	{
		if(ActiveGroups[i]->GetName() == InName)
		{
			ActiveGroups[i]->TrackedAudioComponents.Remove(SceneComponent);
		}
	}
}

bool FGroupedAudioSourcesManager::DoesGroupExist(const FName& InGroupName)
{
	return !!FindGroup(InGroupName);
}

TSharedPtr<FAudioSourcesGroup> FGroupedAudioSourcesManager::FindGroup(const FName& InName)
{
	for (TSharedPtr<FAudioSourcesGroup> ActiveGroup : ActiveGroups)
	{
		if(ActiveGroup->GetName() == InName)
		{
			return ActiveGroup;
		}
	}

	//didn't exist
	return nullptr;
}
