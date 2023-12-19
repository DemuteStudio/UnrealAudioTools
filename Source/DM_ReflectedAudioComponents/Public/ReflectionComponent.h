// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ReflectionSystemConstants.h"
#include "Components/AudioComponent.h"
#include "ReflectionComponent.generated.h"

/**
 * 
 */
UCLASS()
class DM_REFLECTEDAUDIOCOMPONENTS_API UReflectionComponent : public UAudioComponent
{
	GENERATED_BODY()

public:
	
	UReflectionComponent();
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Reflected Audio")
	TEnumAsByte<EReflectionMaterials> ReflectionMaterial;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Reflected Audio")
	int SubReflections = 0;

	UFUNCTION()
	void SetupReflectionComponent(USoundSourceBus* InSoundSourceBus, int InSubReflections = 0);

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Reflected Audio")
	UReflectionComponent* SubReflectionComponent;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Reflected Audio")
	UAudioBus* AudioBus;

	UFUNCTION()
	bool UpdateReflections(FTransform OriginTransform, FVector Direction, float MaxDistance, int subReflectionIndex);

	UFUNCTION()
	void MuteReflection();

private:
	FVector ReflectionNormal;
	void UpdateAudioParameters(FTransform OriginTransform, FVector Location, FReflectionMaterial HitMaterial);
};

