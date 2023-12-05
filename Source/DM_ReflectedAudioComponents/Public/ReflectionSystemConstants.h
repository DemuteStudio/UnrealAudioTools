// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ReflectionSystemConstants.generated.h"


UENUM(BlueprintType)
enum EReflectionMaterials
{
	Default,
	UnglazedBrick,
	RoughConcrete,
	Marble,
	Sand,
	WoodBeams,
	Snow
};

USTRUCT(BlueprintType)
struct FReflectionMaterial
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reflection Material")
	TEnumAsByte<EReflectionMaterials> ReflectionMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reflection Material")
	TArray<float> ReflectionCoefficients;
};

/**
 * 
 */
UCLASS()
class DM_REFLECTEDAUDIOCOMPONENTS_API UReflectionSystemConstants : public UObject
{
public:
	GENERATED_BODY()

	static const TMap<TEnumAsByte<EReflectionMaterials>, FReflectionMaterial> Materials;

	static float ReflectionCoefficientToDbReduction(float ReflectionCoefficient)
	{
		return 20.0f * FMath::LogX(10.0f, (1-ReflectionCoefficient));
	}
};

