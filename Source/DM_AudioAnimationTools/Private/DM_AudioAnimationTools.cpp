// Copyright Epic Games, Inc. All Rights Reserved.

#include "DM_AudioAnimationTools.h"

#define LOCTEXT_NAMESPACE "FDM_AudioAnimationToolsModule"

void FDM_AudioAnimationToolsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FDM_AudioAnimationToolsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDM_AudioAnimationToolsModule, DM_AudioAnimationTools)