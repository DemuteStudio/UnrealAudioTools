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
};

