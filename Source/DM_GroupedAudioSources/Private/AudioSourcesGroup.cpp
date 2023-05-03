// Fill out your copyright notice in the Description page of Project Settings.


#include "AudioSourcesGroup.h"

FAudioSourcesGroup::FAudioSourcesGroup(const FName& InName, const FAudioSourcesGroupSettings& InGroupSettings,
	FGroupedAudioSourcesManager* InOwningGroupManagerPtr)
		: Name(InName)
		, GroupSettings(InGroupSettings)
		, OwningGroupManagerPtr(InOwningGroupManagerPtr)
{
}

FAudioSourcesGroup::~FAudioSourcesGroup()
{
}

void FAudioSourcesGroup::Tick(float DeltaTime)
{
	
}

bool FAudioSourcesGroup::DoesMatchSettings(const FAudioSourcesGroupSettings& InGroupSettings) const
{
	//TODO: implement this
	return true;
}

void FAudioSourcesGroup::ChangeGroupSettings(const FAudioSourcesGroupSettings& InNewSettings)
{
}
