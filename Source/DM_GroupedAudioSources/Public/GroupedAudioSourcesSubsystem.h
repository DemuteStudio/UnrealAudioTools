// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GroupedAudioSourcesManager.h"
#include "Subsystems/WorldSubsystem.h"
#include "GroupedAudioSourcesUtilities.h"
#include "GroupedAudioSourcesSubsystem.generated.h"

class UAudioSourcesGroupHandle;
/**
 * Subsystem that handles the grouped audio sources. Heavily inspired from the Quartz subsystem.
 */
UCLASS()
class DM_GROUPEDAUDIOSOURCES_API UGroupedAudioSourcesSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:

	// ctor/dtor
	UGroupedAudioSourcesSubsystem();
	~UGroupedAudioSourcesSubsystem();

	//~ Begin UWorldSubsystem Interface
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;
	//~ End UWorldSubsystem Interface

	//~ Begin FTickableGameObject Interface
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	//~ End FTickableGameObject Interface

	//static methods
	static UGroupedAudioSourcesSubsystem* Get(UWorld* World);

	//Register a new audio source
	UFUNCTION(BlueprintCallable, Category = "Grouped Audio Sources", meta = (WorldContext = "WorldContextObject"))
	UAudioSourcesGroupHandle* RegisterNewAudioSource(const UObject* WorldContextObject, FName GroupName, USceneComponent* AudioComponent, FAudioSourcesGroupSettings InSettings, bool bOverrideSettingsIfGroupExists = false);

	//Method that will make it possible to handle the manager on different threads in the future.
	FGroupedAudioSourcesManager* GetManagerForClock(const UObject* WorldContextObject, FName ExistingGroupName = FName());

	
private:
	FGroupedAudioSourcesManager SubsystemGroupsManager;
};
