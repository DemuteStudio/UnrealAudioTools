// Fill out your copyright notice in the Description page of Project Settings.



#include "Kismet/GameplayStatics.h"
#include "GroupedAudioSourcesManager.h"

FGroupedAudioSourcesManager::FGroupedAudioSourcesManager(UGroupedAudioSourcesSubsystem* InOwner) : Subsystem(InOwner)
{
}

FGroupedAudioSourcesManager::~FGroupedAudioSourcesManager()
{
}

void FGroupedAudioSourcesManager::Tick(float DeltaTime)
{
	
	//Store listener location here so that groups can all reference it
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(Subsystem->GetWorld(), 0);
	if(!PlayerController) return;
	PlayerController->GetAudioListenerAttenuationOverridePosition(CurrentListenerPosition);
	
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
                                                                                   UGroupedAudioComponent* AudioComponent, const FAudioSourcesGroupSettings& InSettings, bool bOverrideSettingsIfGroupExists)
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

void FGroupedAudioSourcesManager::UnregisterAudioSource(const FName& InName, UGroupedAudioComponent* AudioComponent)
{
	const int32 NumGroups = ActiveGroups.Num();
	for (int32 i = NumGroups - 1; i>=0; --i)
	{
		if(ActiveGroups[i]->GetName() == InName)
		{
			ActiveGroups[i]->TrackedAudioComponents.Remove(AudioComponent);
		}
	}
}

bool FGroupedAudioSourcesManager::DoesGroupExist(const FName& InGroupName)
{
	return !!FindGroup(InGroupName);
}

int FGroupedAudioSourcesManager::GetActiveSourcesCountForGroup(const FName& InGroupName)
{
	const TSharedPtr<FAudioSourcesGroup> Group = FindGroup(InGroupName);

	return Group->GetActiveSourcesCount();

}

int FGroupedAudioSourcesManager::GetActiveClustersCountForGroup(const FName& InGroupName)
{
	const TSharedPtr<FAudioSourcesGroup> Group = FindGroup(InGroupName);

	return Group->GetActiveClustersCount();
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
