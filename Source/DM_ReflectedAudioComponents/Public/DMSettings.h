// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ReflectionSystemConstants.h"
#include "UObject/Object.h"
#include "DMSettings.generated.h"

/**
 * 
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "DM Audio Tools Settings"))
class DM_REFLECTEDAUDIOCOMPONENTS_API UDMSettings : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, Category = "Reflections Material Map")
	TMap<TSoftObjectPtr<UPhysicalMaterial>, TEnumAsByte<EReflectionMaterials>> ReflectionsMaterialMap;
	
	static FReflectionMaterial GetReflectionMaterial(TWeakObjectPtr<UPhysicalMaterial>::ElementType* Get);
};
