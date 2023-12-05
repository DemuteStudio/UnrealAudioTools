// Fill out your copyright notice in the Description page of Project Settings.


#include "DMSettings.h"

FReflectionMaterial UDMSettings::GetReflectionMaterial(TWeakObjectPtr<UPhysicalMaterial>::ElementType* Get)
{
	//Get current settings
	if(const UDMSettings* Settings = GetDefault<UDMSettings>())
	{
		if(const TEnumAsByte<EReflectionMaterials>* ReflectionMaterial = Settings->ReflectionsMaterialMap.Find(Get))
		{
			return UReflectionSystemConstants::Materials.FindChecked(*ReflectionMaterial);
		}
	}

	return UReflectionSystemConstants::Materials.FindChecked(Default);
}
