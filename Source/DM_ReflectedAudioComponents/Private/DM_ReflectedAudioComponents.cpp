#include "DM_ReflectedAudioComponents.h"

#include "DMSettings.h"
#include "Developer/Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FDM_ReflectedAudioComponentsModule"

void FDM_ReflectedAudioComponentsModule::StartupModule()
{
	const auto SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	SettingsModule->RegisterSettings("Project", "DM_Audio", "Reflection System",
	                                 LOCTEXT("DMAudioSettingsName", "Reflection System Settings"),
	                                 LOCTEXT("DMAudioSettingsDescription", "Set up the reflection system settings"),
	                                 GetMutableDefault<UDMSettings>());
}

void FDM_ReflectedAudioComponentsModule::ShutdownModule()
{
	const auto SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	SettingsModule->UnregisterSettings("Project", "DM_Audio", "Reflection System");
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDM_ReflectedAudioComponentsModule, DM_ReflectedAudioComponents)
