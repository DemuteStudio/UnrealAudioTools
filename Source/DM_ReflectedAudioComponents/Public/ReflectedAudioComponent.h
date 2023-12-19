﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "ReflectedAudioComponent.generated.h"

class UReflectionComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DM_REFLECTEDAUDIOCOMPONENTS_API UReflectedAudioComponent : public UAudioComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UReflectedAudioComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reflected Audio")
	bool bReflectionsEnabled = true;
	
	/**
	 * The amount of units that the component can move before we recalculate the reflections.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Reflected Audio")
	float ReflectionMovementThreshold = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Reflected Audio")
	int MaxReflections = 8;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Reflected Audio")
	float MaxReflectionDistance = 1000.0f;

	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Reflected Audio")
	// float ReflectionInterpolationSpeed = 100.0f;
	
	// UPROPERTY(Transient, BlueprintReadOnly, Category = "Reflected Audio")
	// TArray<FVector> ReflectionPositions;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Reflected Audio")
	TArray<UReflectionComponent*> ReflectionAudioComponents;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Reflected Audio")
	UAudioBus* AudioBus;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Reflected Audio", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float HorizontalRaycastSkewing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Reflected Audio")
	int SubReflections = 0;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport) override;

private:

	FVector PreviousLocation;
	int currentReflectionIndex = 0;
	int currentSubReflectionIndex = 0;
	bool needsToRecalculateReflections = false;
	TArray<FVector> ReflectionDirections;
	bool areReflectionsEnabled = false;
};
