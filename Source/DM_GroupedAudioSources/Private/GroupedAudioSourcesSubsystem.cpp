// Fill out your copyright notice in the Description page of Project Settings.


#include "GroupedAudioSourcesSubsystem.h"

#include "AudioSourcesGroupHandle.h"

UGroupedAudioSourcesSubsystem::UGroupedAudioSourcesSubsystem() : SubsystemGroupsManager(this)
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

	AudioSourcesGroups.AddUnique(GroupName);
	
	if(!GroupsManager)
	{
		return nullptr;
	}

	GroupsManager->RegisterNewAudioSource(GroupName, AudioComponent, InSettings, bOverrideSettingsIfGroupExists);

	UAudioSourcesGroupHandle* GroupHandlePtr = NewObject<UAudioSourcesGroupHandle>()->Init(WorldContextObject->GetWorld())->SubscribeToGroup(WorldContextObject, GroupName);
	return GroupHandlePtr;
}

void UGroupedAudioSourcesSubsystem::UnregisterAudioSource(const UObject* WorldContextObject,
	FName GroupName, USceneComponent* AudioComponent)
{
	FGroupedAudioSourcesManager* GroupsManager = GetManagerForClock(WorldContextObject, GroupName);
	if(!GroupsManager)
	{
		return;
	}

	GroupsManager->UnregisterAudioSource(GroupName, AudioComponent);
	
}

UAudioSourcesGroupHandle* UGroupedAudioSourcesSubsystem::GetHandleForAudioGroup(const UObject* WorldContextObject,
	FName GroupName)
{
	FGroupedAudioSourcesManager* GroupsManager = GetManagerForClock(WorldContextObject, GroupName);
	if(!GroupsManager)
	{
		return nullptr;
	}

	if(GroupsManager->DoesGroupExist(GroupName))
	{
		return NewObject<UAudioSourcesGroupHandle>()->Init(WorldContextObject->GetWorld())->SubscribeToGroup(WorldContextObject,GroupName);
	}

	return nullptr;
}

TArray<UAudioSourcesGroupHandle*> UGroupedAudioSourcesSubsystem::GetHandlesForAllAudioGroups(
	const UObject* WorldContextObject)
{
	//I'm not sure how to tackle this, so for the moment I'll first check if there is handle existing for every group,
	//then create new ones for the groups that don't have one. This is because I'd like to be able to call this often
	//for profiling purposes.

	TArray<UAudioSourcesGroupHandle*> Handles;
	for (FName GroupName : AudioSourcesGroups)
	{
		UAudioSourcesGroupHandle* GroupHandle = *GroupedAudioSourcesSubscribers.FindByPredicate(
			[GroupName](const UAudioSourcesGroupHandle* handle)
			{
				return (handle->GetGroupName() == GroupName);
			});

		if(!GroupHandle)
		{
			GroupHandle = GetHandleForAudioGroup(WorldContextObject, GroupName);
		}

		Handles.Add(GroupHandle);
	}

	return Handles;
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

void UGroupedAudioSourcesSubsystem::SubscribeToGroupedAudioSourcesTick(UAudioSourcesGroupHandle* InObjectToTick)
{
	if(!InObjectToTick)
	{
		return;
	}

	GroupedAudioSourcesSubscribers.AddUnique(InObjectToTick);
}
