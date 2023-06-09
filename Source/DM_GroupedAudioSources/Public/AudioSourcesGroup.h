// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GroupedAudioSourcesUtilities.h"

// forwards
class FGroupedAudioSourcesManager;
/**
 * 
 */
class DM_GROUPEDAUDIOSOURCES_API FAudioSourcesGroup
{
public:
	//ctor
	FAudioSourcesGroup(const FName& InName, const FAudioSourcesGroupSettings& InGroupSettings, FGroupedAudioSourcesManager* InOwningGroupManagerPtr = nullptr);
	~FAudioSourcesGroup();
	void Tick(float DeltaTime);
	
	FName GetName() const {return Name;}
	
	bool DoesMatchSettings(const FAudioSourcesGroupSettings& InGroupSettings) const;
	void ChangeGroupSettings(const FAudioSourcesGroupSettings& InNewSettings);
	int GetActiveSourcesCount() const;

	TArray<USceneComponent*> TrackedAudioComponents;

private:

	FName Name;
	FAudioSourcesGroupSettings GroupSettings;
	FGroupedAudioSourcesManager* OwningGroupManagerPtr {nullptr};
};