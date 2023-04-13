// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "AudioAnimationToolsWidget.generated.h"

USTRUCT(BlueprintType)
struct FFootstepAudioData
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	FName BoneName;

	UPROPERTY(BlueprintReadOnly)
	float FootstepSpeed;

	UPROPERTY(BlueprintReadOnly)
	float FootstepTime;
};

USTRUCT(BlueprintType)
struct FFootstepAudioTrack
{

	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FName TrackName;

	UPROPERTY(BlueprintReadOnly)
	TArray<FFootstepAudioData> Footsteps;

	/**Used for debug purposes*/
	UPROPERTY(BlueprintReadOnly)
	float LowestBonePosition;

	/**Used for debug purposes*/
	UPROPERTY(BlueprintReadOnly)
	float HighestBonePosition;

	/**Used for filtering purposes*/
	UPROPERTY(BlueprintReadOnly)
	float FastestBoneSpeed;

	/**Used for filtering purposes*/
	UPROPERTY(BlueprintReadOnly)
	float SlowestBoneSpeed;
};

USTRUCT(BlueprintType)
struct FFoleyAudioData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly)
	FName BoneName;

	UPROPERTY(BlueprintReadOnly)
	float FoleySpeed;

	UPROPERTY(BlueprintReadOnly)
	float FoleyLength;

	UPROPERTY(BlueprintReadOnly)
	float FoleyTime;
	
};

USTRUCT(BlueprintType)
struct FFoleyAudioTrack
{

	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FName TrackName;

	UPROPERTY(BlueprintReadOnly)
	TArray<FFoleyAudioData> FoleyMovements;
};


/**
 * 
 */
UCLASS(BlueprintType)
class DM_AUDIOANIMATIONTOOLS_API UAudioAnimationToolsWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	//Blueprint methods
	UFUNCTION(BlueprintCallable)
	void AutoGenerateFootstepNotifies(UAnimSequence* AnimationSequence, TArray<FName> BoneNames, TMap<UAnimNotify*, FFootstepAudioData>& CreatedNotifies, TArray
	                                  <FFootstepAudioTrack>& CreatedTracks);

	UFUNCTION(BlueprintCallable)
	void AutoGenerateFoleyNotifies(UAnimSequence* AnimationSequence, TArray<FName> BoneNames, TMap<UAnimNotify*, FFoleyAudioData>& CreatedNotifies, TArray
	                               <FFoleyAudioTrack>& CreatedTracks);

	//User modifiable variables
	/**Height under which a foot is considered to touch the ground*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FootstepDetails")
	float GroundContactStartThreshold = 3.0f;

	/**Height added to the GroundContactStartThreshold above which a foot is considered to not touch the ground anymore*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FootstepDetails")
	float GroundContactStopMargin = 3.0f;

	/**The time in seconds used to calculate the speed of the foot just before a footstep (to derive its strength)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FootstepDetails")
	float FootstepSpeedCalculationWindow = 0.1f;

	/**The percentage of footstep detection sensitivity*/
	/**
	 * This deserves a bit of explanation. Currently the "sensitivity" works as follows.
	 * We take the lowest and highest footstep speeds encountered throughout the analysis.
	 * Then we filter out all the footsteps whose speed is lower than the MaxSpeed*(1-Sensitivity).
	 * For example if we set this as 50%, we are not filtering 50% of the footsteps.
	 * We are filtering the footsteps that go slower than 50% of the max speed.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FootstepDetails", meta = (ClampMin = "0.0", ClampMax ="1.0", UIMin ="0.0", UIMax ="1.0"))
	float FootstepDetectionSensitivity = 1.0f;

	/**In units per second, the speed above which a bone is considered to be moving*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FoleyDetails")
	float FoleySpeedThreshold = 10.0f;

	/**The time in seconds under which a movement will be considered too short to trigger*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FoleyDetails")
	float FoleyMinimumMovementDuration = 0.05f;
	
	/**Class to use for newly created AnimNotifies (lets you use custom notifies)*/
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAnimNotify> AnimNotifyClass;

	/**Should the anim tool spam the log with information*/
	UPROPERTY(EditAnywhere, Category="Debug")
	bool VerboseLogging = false;
	
	
private:

	//System variables
	const float AnimationTimeIncrement = 1.0f / 60.0f;
	
	//System methods
	static int GetAnimationTrackIndex(int SkeletonBoneIndex, const UAnimSequence* AnimSequence);

	static FVector GetBoneLocation(const UAnimSequence* AnimSequence, const FReferenceSkeleton& ReferenceSkeleton, int BoneIndex, float Time);

	static void GetWorldToRootTransform(const UAnimSequence* AnimSequence, float Time, FTransform& OutTransform);

	static void GetRootToBoneTransform(const UAnimSequence* AnimSequence, const FReferenceSkeleton& ReferenceSkeleton, int BoneIndex, float Time, FTransform&
	                                   OutTransform);
};
