// Fill out your copyright notice in the Description page of Project Settings.


#include "ReflectionSystemConstants.h"

const TMap<TEnumAsByte<EReflectionMaterials>, FReflectionMaterial> UReflectionSystemConstants::Materials =
	{
		{Default, FReflectionMaterial{Default,
			{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}},
		{UnglazedBrick, FReflectionMaterial{UnglazedBrick,
			{0.3f, 0.03f, 0.03f, 0.03f, 0.04f, 0.05f, 0.07f, 0.08f}}},
		{RoughConcrete, FReflectionMaterial{RoughConcrete,
			{0.1f, 0.02f, 0.03f, 0.03f, 0.03f, 0.04f, 0.07f, 0.08f}}},
		{Marble, FReflectionMaterial{Marble,
			{0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.02f, 0.02f, 0.03f}}},
		{Sand, FReflectionMaterial{Sand,
			{0.05f, 0.15f, 0.35f, 0.4f, 0.5f, 0.55f, 0.8f, 0.95f}}},
		{WoodBeams, FReflectionMaterial{WoodBeams,
			{0.2f, 0.15f, 0.11f, 0.1f, 0.07f, 0.06f, 0.07f, 0.08f}}},
		{Snow, FReflectionMaterial{Snow,
			{0.1f, 0.09f, 0.2f, 0.45f, 0.9f, 0.85f, 0.8f, 0.75f}}},
	};
	
