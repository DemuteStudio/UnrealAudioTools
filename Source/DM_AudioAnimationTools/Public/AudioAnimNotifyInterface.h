// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AudioAnimNotifyInterface.generated.h"

/**
 * 
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UAudioAnimNotifyInterface : public UInterface
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FAudioAnimNotifyParameter
{

	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "AnimNotifyInterface")
	FName ParameterName;
	
	UPROPERTY(BlueprintReadWrite, Category = "AnimNotifyInterface")
	float ParameterValue;
};

class IAudioAnimNotifyInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AnimNotifyInterface")
	bool SetSound(UObject* SoundObject);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AnimNotifyInterface")
	bool SetFollow(const bool bFollow);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AnimNotifyInterface")
	bool SetAttachName(FName AttachName);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AnimNotifyInterface")
	bool SetParameters(const TArray<FAudioAnimNotifyParameter>& ParametersObject);
};
