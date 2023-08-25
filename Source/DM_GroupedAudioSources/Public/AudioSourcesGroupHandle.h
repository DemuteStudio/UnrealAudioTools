// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AudioSourcesGroupHandle.generated.h"

class UGroupedAudioSourcesSubsystem;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, Transient, ClassGroup = GroupedAudioSources, meta = (BlueprintSpawnableComponent))
class DM_GROUPEDAUDIOSOURCES_API UAudioSourcesGroupHandle : public UObject
{
	GENERATED_BODY()

public:
	UAudioSourcesGroupHandle* Init(UWorld* InWorldPtr);

	UFUNCTION(BlueprintCallable, Category = "Audio Sources Group")
	FName GetGroupName() const { return CurrentGroupId; }

	UFUNCTION(BlueprintCallable, Category = "Audio Sources Group", meta = (WorldContext = "WorldContextObject"))
	int GetNumberOfActiveSources(const UObject* WorldContextObject) const;
	
	UFUNCTION(BlueprintCallable, Category = "Audio Sources Group", meta = (WorldContext = "WorldContextObject"))
	int GetNumberOfActiveClusters(const UObject* WorldContextObject) const;
	
	UAudioSourcesGroupHandle* SubscribeToGroup(const UObject* WorldContextObject, FName GroupName);

private:
	UPROPERTY(Transient)
	UWorld* WorldPtr{nullptr};

	UPROPERTY(Transient)
	UGroupedAudioSourcesSubsystem* GroupedAudioSourcesSubsystem;

	FName CurrentGroupId;
	FName GroupHandleId;
	bool bConnectedToGroup{false};
};
