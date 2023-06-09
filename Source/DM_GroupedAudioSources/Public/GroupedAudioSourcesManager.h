// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GroupedAudioSourcesUtilities.h"
#include "AudioSourcesGroup.h"
#include "AudioSourcesGroupHandle.h"

class UGroupedAudioSourcesSubsystem;
/**
 * 
 */
class DM_GROUPEDAUDIOSOURCES_API FGroupedAudioSourcesManager
{
public:
	FGroupedAudioSourcesManager(UGroupedAudioSourcesSubsystem* InOwner = nullptr);
	~FGroupedAudioSourcesManager();

	void Tick(float DeltaTime);
	
	void Shutdown();
	void Flush();
	int32 GetNumGroups() const {return ActiveGroups.Num();}
	TSharedPtr<FAudioSourcesGroup> RegisterNewAudioSource(const FName& InGroupName, USceneComponent* AudioComponent, const FAudioSourcesGroupSettings& InSettings, bool bOverrideSettingsIfGroupExists = false);
	void UnregisterAudioSource(const FName& InName, USceneComponent* SceneComponent);
	bool DoesGroupExist(const FName& InGroupName);
	int GetActiveSourcesCountForGroup(const FName& InGroupName);

private:
	TArray<TSharedPtr<FAudioSourcesGroup>> ActiveGroups;

	TSharedPtr<FAudioSourcesGroup> FindGroup(const FName& InName);

	//Pointer to owner subsystem
	UGroupedAudioSourcesSubsystem* Subsystem;

};
