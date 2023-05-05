// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GroupedAudioSourcesUtilities.h"
#include "AudioSourcesGroup.h"

/**
 * 
 */
class DM_GROUPEDAUDIOSOURCES_API FGroupedAudioSourcesManager
{
public:
	FGroupedAudioSourcesManager();
	~FGroupedAudioSourcesManager();

	void Tick(float DeltaTime);
	
	void Shutdown();
	void Flush();
	int32 GetNumGroups() const {return ActiveGroups.Num();}
	TSharedPtr<FAudioSourcesGroup> RegisterNewAudioSource(const FName& InGroupName, USceneComponent* AudioComponent, const FAudioSourcesGroupSettings& InSettings, bool bOverrideSettingsIfGroupExists = false);
	void UnregisterAudioSource(const FName& InName, USceneComponent* SceneComponent);
	bool DoesGroupExist(const FName& InGroupName);

private:
	TArray<TSharedPtr<FAudioSourcesGroup>> ActiveGroups;

	TSharedPtr<FAudioSourcesGroup> FindGroup(const FName& InName);

};
