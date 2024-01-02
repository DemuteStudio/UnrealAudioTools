// Fill out your copyright notice in the Description page of Project Settings.


#include "AudioAnimationToolsWidget.h"

#include "Animation/AnimNotifies/AnimNotify.h"

void UAudioAnimationToolsWidget::AutoGenerateFootstepNotifies(UAnimSequence* AnimationSequence, TArray<FName> BoneNames,TSubclassOf<UAnimNotify> AnimNotifyClass,
                                                              TMap<UAnimNotify*, FFootstepAudioData>& CreatedNotifies, TArray<FFootstepAudioTrack>& CreatedTracks)
{
	CreatedNotifies.Empty();
	
	CreatedTracks.Empty();

	switch (FootstepDetectionMethod) {
	case HeightThreshold:
		CalculateFootstepsWithHeightThreshold(AnimationSequence, BoneNames, CreatedTracks);
		break;
	case TangentCalculation:
		CalculateFootstepsWithTangents(AnimationSequence, BoneNames, CreatedTracks);
		break;
	}

#if WITH_EDITORONLY_DATA

	bool WasAnimationFileModified = false;

	for (FFootstepAudioTrack& FootstepAudioTrack : CreatedTracks)
	{
		int NewNotifyTrackIndex = AnimationSequence->AnimNotifyTracks.Num();

		//If a track already exists with same name clear it (suppose previous generation)
		for (int TrackId = 0; TrackId < AnimationSequence->AnimNotifyTracks.Num(); TrackId++)
		{
			if(AnimationSequence->AnimNotifyTracks[TrackId].TrackName == FootstepAudioTrack.TrackName)
			{
				AnimationSequence->Notifies.RemoveAll([TrackId](const FAnimNotifyEvent& NotifyEvent){return NotifyEvent.TrackIndex == TrackId;});
				NewNotifyTrackIndex = TrackId;
				WasAnimationFileModified = true;
			}
		}

		
		if(AnimationSequence->AnimNotifyTracks.Num() <= NewNotifyTrackIndex)
		{
			AnimationSequence->AnimNotifyTracks.SetNum(NewNotifyTrackIndex + 1);
			AnimationSequence->AnimNotifyTracks[NewNotifyTrackIndex].TrackName = FootstepAudioTrack.TrackName;
			WasAnimationFileModified = true;
		}
		
		//Sensitivity calculation
		const float MinimumSpeed = FootstepAudioTrack.FastestBoneSpeed*(1-FootstepDetectionSensitivity);
		
		for (FFootstepAudioData& FootstepAudioData : FootstepAudioTrack.Footsteps)
		{
			//Sensitivity filtering
			if(FootstepAudioData.FootstepSpeed <= MinimumSpeed) continue;
			//Filter out events on animation edges
			if(FootstepAudioData.FootstepTime <= AnimationEdgesMargin ||
				FootstepAudioData.FootstepTime >= AnimationSequence->GetPlayLength() - AnimationEdgesMargin) continue;
			
			FAnimNotifyEvent NewNotifyEvent;
			NewNotifyEvent.NotifyName = "AutoTag_Footstep";
			UAnimNotify* Notify = NewObject<UAnimNotify>(AnimationSequence, AnimNotifyClass);
			NewNotifyEvent.Notify = Notify;
			NewNotifyEvent.SetTime(FootstepAudioData.FootstepTime);
			NewNotifyEvent.TrackIndex = NewNotifyTrackIndex;

			AnimationSequence->Notifies.Add(NewNotifyEvent);
			CreatedNotifies.Add(Notify, FootstepAudioData);
			
			WasAnimationFileModified = true;
		}
	}

	if(WasAnimationFileModified)
	{
		AnimationSequence->Modify(true);
		AnimationSequence->RefreshCacheData();
	}
	
#endif
}

void UAudioAnimationToolsWidget::AutoGenerateFoleyNotifies(UAnimSequence* AnimationSequence, TArray<FName> BoneNames,TSubclassOf<UAnimNotify> AnimNotifyClass,
	TMap<UAnimNotify*, FFoleyAudioData>& CreatedNotifies, TArray<FFoleyAudioTrack>& CreatedTracks)
{
	CreatedNotifies.Empty();

	if(AnimationSequence == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AudioAnimationToolsWidget -- AutoGenerateFoleyNotifies : passed a null AnimSequence. Early exit."));
		return;
	}

	const USkeleton* Skeleton = AnimationSequence->GetSkeleton();

	if(Skeleton == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AudioAnimationToolsWidget -- AutoGenerateFoleyNotifies : AnimSequence had a null skeleton. Early exit."));
		return;
	}

	const FReferenceSkeleton& ReferenceSkeleton = Skeleton->GetReferenceSkeleton();

	TMap<FName, int> BoneIndexesMap;
	for (FName BoneName : BoneNames)
	{
		const int BoneIndex = ReferenceSkeleton.FindBoneIndex(BoneName);
		if(BoneIndex != INDEX_NONE)
		{
			BoneIndexesMap.Add(BoneName, BoneIndex);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AudioAnimationToolsWidget -- AutoGenerateFoleyNotifies : Bone %s does not exist, skipping it"), *BoneName.ToString());
		}
	}
	
	CreatedTracks.Empty();
	
	for (const TTuple<FName, int>& Bone : BoneIndexesMap)
	{
		FFoleyAudioTrack& NewFoleyTrack = CreatedTracks.AddDefaulted_GetRef();
		NewFoleyTrack.TrackName = FName(*("AutoGen " + Bone.Key.ToString()));

		float MovingSinceTime = -1.0f;
		float PreviousBoneSpeed = 0.0f;
		FVector PreviousBoneLocation = FVector::ZeroVector;
		for (float Time = AnimationTimeIncrement; Time < AnimationSequence->GetPlayLength(); Time+=AnimationTimeIncrement)
		{
			FVector BoneLocation = GetBoneLocation(AnimationSequence, ReferenceSkeleton, Bone.Value, Time);
			const float BoneSpeed = ((BoneLocation - PreviousBoneLocation) / AnimationTimeIncrement).Size();

			const bool Moving = BoneSpeed >= FoleySpeedThreshold && BoneSpeed >= PreviousBoneSpeed;

			if(Moving && MovingSinceTime < 0.0f) //Started moving
			{
				MovingSinceTime = Time;
			}
			else if(!Moving && MovingSinceTime >= 0.0f) //Stopped moving
			{
				const float MovementDuration = Time - MovingSinceTime;

				if(MovementDuration >= FoleyMinimumMovementDuration)
				{
					FFoleyAudioData& NewFoleyAudioData = NewFoleyTrack.FoleyMovements.AddDefaulted_GetRef();
					NewFoleyAudioData.BoneName = Bone.Key;
					NewFoleyAudioData.FoleyTime = MovingSinceTime;
					NewFoleyAudioData.FoleyLength = MovementDuration;
					NewFoleyAudioData.FoleySpeed = PreviousBoneSpeed;
				}

				MovingSinceTime = -1.0f;
			}

			PreviousBoneLocation = BoneLocation;
			PreviousBoneSpeed = BoneSpeed;
		}
	}
	
#if WITH_EDITORONLY_DATA

	bool WasAnimationFileModified = false;

	for (FFoleyAudioTrack& FoleyAudioTrack : CreatedTracks)
	{
		int NewNotifyTrackIndex = AnimationSequence->AnimNotifyTracks.Num();

		//If a track already exists with same name clear it (suppose previous generation)
		for (int TrackId = 0; TrackId < AnimationSequence->AnimNotifyTracks.Num(); TrackId++)
		{
			if(AnimationSequence->AnimNotifyTracks[TrackId].TrackName == FoleyAudioTrack.TrackName)
			{
				AnimationSequence->Notifies.RemoveAll([TrackId](const FAnimNotifyEvent& NotifyEvent){return NotifyEvent.TrackIndex == TrackId;});
				NewNotifyTrackIndex = TrackId;
				WasAnimationFileModified = true;
			}
		}

		if(AnimationSequence->AnimNotifyTracks.Num() <= NewNotifyTrackIndex)
		{
			AnimationSequence->AnimNotifyTracks.SetNum(NewNotifyTrackIndex + 1);
			AnimationSequence->AnimNotifyTracks[NewNotifyTrackIndex].TrackName = FoleyAudioTrack.TrackName;
			WasAnimationFileModified = true;
		}


		for (FFoleyAudioData& FoleyAudioData : FoleyAudioTrack.FoleyMovements)
		{
			FAnimNotifyEvent NewNotifyEvent;
			NewNotifyEvent.NotifyName = "AutoTag_Foley";
			UAnimNotify* Notify = NewObject<UAnimNotify>(AnimationSequence, AnimNotifyClass);
			NewNotifyEvent.Notify = Notify;
			NewNotifyEvent.SetTime(FoleyAudioData.FoleyTime);
			NewNotifyEvent.TrackIndex = NewNotifyTrackIndex;
			
			AnimationSequence->Notifies.Add(NewNotifyEvent);
			CreatedNotifies.Add(Notify, FoleyAudioData);
			
			WasAnimationFileModified = true;
		}
	}

	if(WasAnimationFileModified)
	{
		AnimationSequence->Modify(true);
		AnimationSequence->RefreshCacheData();
	}
	
#endif
}

void UAudioAnimationToolsWidget::CalculateFootstepsWithHeightThreshold(UAnimSequence* AnimationSequence,
	TArray<FName> BoneNames, TArray<FFootstepAudioTrack>& FootstepTracks)
{
	if(AnimationSequence == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AudioAnimationToolsWidget -- AutoGenerateFootstepNotifies : passed a null AnimSequence. Early exit."));
		return;
	}

	const USkeleton* Skeleton = AnimationSequence->GetSkeleton();

	if(Skeleton == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AudioAnimationToolsWidget -- AutoGenerateFootstepNotifies : AnimSequence had a null skeleton. Early exit."));
		return;
	}

	const FReferenceSkeleton& ReferenceSkeleton = Skeleton->GetReferenceSkeleton();

	TMap<FName, int> BoneIndexesMap;
	for (FName BoneName : BoneNames)
	{
		const int BoneIndex = ReferenceSkeleton.FindBoneIndex(BoneName);
		if(BoneIndex != INDEX_NONE)
		{
			BoneIndexesMap.Add(BoneName, BoneIndex);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AudioAnimationToolsWidget -- AutoGenerateFootstepNotifies : Bone %s does not exist, skipping it"), *BoneName.ToString());
		}
	}
	
	FootstepTracks.Empty();

	for (const TTuple<FName, int>& Bone : BoneIndexesMap)
	{
		FFootstepAudioTrack& NewFootstepTrack = FootstepTracks.AddDefaulted_GetRef();
		NewFootstepTrack.TrackName = FName(*("AutoGen " + Bone.Key.ToString()));

		float GroundContactSinceTime = 0.0f; //Assume feet start on ground
		//Debug values tracking
		NewFootstepTrack.HighestBonePosition = GetBoneLocation(AnimationSequence, ReferenceSkeleton, Bone.Value, 0).Z;;
		NewFootstepTrack.LowestBonePosition = NewFootstepTrack.HighestBonePosition;
		NewFootstepTrack.FastestBoneSpeed = 0.0f;
		NewFootstepTrack.SlowestBoneSpeed = -1.0f;
		
		for (float Time = AnimationTimeIncrement; Time < AnimationSequence->GetPlayLength(); Time += AnimationTimeIncrement)
		{
			const FVector FootLocation = GetBoneLocation(AnimationSequence, ReferenceSkeleton, Bone.Value, Time);

			const bool GroundContact = GroundContactSinceTime < 0.0f? FootLocation.Z <= GroundContactStartThreshold : FootLocation.Z <= GroundContactStartThreshold + GroundContactStopMargin;

			if(FootLocation.Z > NewFootstepTrack.HighestBonePosition) NewFootstepTrack.HighestBonePosition = FootLocation.Z;
			if(FootLocation.Z < NewFootstepTrack.LowestBonePosition) NewFootstepTrack.LowestBonePosition = FootLocation.Z;
			
			if(VerboseLogging)
			{
				UE_LOG(LogTemp, Log, TEXT("AudioAnimationTools : Bone %s height on time %f was %f units."), *Bone.Key.ToString(), Time, FootLocation.Z);
			}
			
			if(GroundContact && GroundContactSinceTime < 0.0f) //New footstep!
			{

				if(VerboseLogging)
				{
					UE_LOG(LogTemp, Log, TEXT("AudioAnimationTools : Bone %s did a footstep on time %f"), *Bone.Key.ToString(), Time);
				}
				
				GroundContactSinceTime = Time;

				FVector PastFootLocation = GetBoneLocation(AnimationSequence, ReferenceSkeleton, Bone.Value, Time-FootstepSpeedCalculationWindow);
				const float FootSpeed = ((FootLocation - PastFootLocation)/FootstepSpeedCalculationWindow).Size();
				FFootstepAudioData& NewFootstep = NewFootstepTrack.Footsteps.AddDefaulted_GetRef();
				NewFootstep.BoneName = Bone.Key;
				NewFootstep.FootstepTime = Time;
				NewFootstep.FootstepSpeed = FootSpeed;

				//Values for filtering
				if(FootSpeed < NewFootstepTrack.SlowestBoneSpeed || NewFootstepTrack.SlowestBoneSpeed == -1.0f) NewFootstepTrack.SlowestBoneSpeed = FootSpeed;
				if(FootSpeed > NewFootstepTrack.FastestBoneSpeed) NewFootstepTrack.FastestBoneSpeed = FootSpeed;
			}
			else if(!GroundContact && GroundContactSinceTime >= 0.0f) //Foot stopped touching ground
			{
				GroundContactSinceTime = -1.0f;

				if(VerboseLogging)
				{
					UE_LOG(LogTemp, Log, TEXT("AudioAnimationTools : Bone %s stopped touching ground on time %f"), *Bone.Key.ToString(), Time);
				}
			}
		}
	}
}

void UAudioAnimationToolsWidget::CalculateFootstepsWithTangents(UAnimSequence* AnimationSequence,
                                                                TArray<FName> BoneNames, TArray<FFootstepAudioTrack>& FootstepTracks)
{
	FootstepTracks.Empty();

	if(AnimationSequence == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AudioAnimationToolsWidget -- CalculateFootstepsWithTangents : passed a null AnimSequence. Early exit."));
		return;
	}

	const USkeleton* Skeleton = AnimationSequence->GetSkeleton();

	if(Skeleton == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AudioAnimationToolsWidget -- CalculateFootstepsWithTangents : AnimSequence had a null skeleton. Early exit."));
		return;
	}

	const FReferenceSkeleton& ReferenceSkeleton = Skeleton->GetReferenceSkeleton();

	TMap<FName, int> BoneIndexesMap;
	for (FName BoneName : BoneNames)
	{
		const int BoneIndex = ReferenceSkeleton.FindBoneIndex(BoneName);
		if(BoneIndex != INDEX_NONE)
		{
			BoneIndexesMap.Add(BoneName, BoneIndex);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AudioAnimationToolsWidget -- CalculateFootstepsWithTangents : Bone %s does not exist, skipping it"), *BoneName.ToString());
		}
	}

	for (const TTuple<FName, int>& Bone : BoneIndexesMap)
	{
		FFootstepAudioTrack& NewFootstepTrack = FootstepTracks.AddDefaulted_GetRef();
		NewFootstepTrack.TrackName = FName(*("AutoGen " + Bone.Key.ToString()));

		//Debug values tracking
		NewFootstepTrack.HighestBonePosition = GetBoneLocation(AnimationSequence, ReferenceSkeleton, Bone.Value, 0).Z;;
		NewFootstepTrack.LowestBonePosition = NewFootstepTrack.HighestBonePosition;
		NewFootstepTrack.FastestBoneSpeed = 0.0f;
		NewFootstepTrack.SlowestBoneSpeed = -1.0f;
		
		bool wasGoingDown = false;
		FVector PreviousBoneLocation = FVector::ZeroVector;
		for (float Time = AnimationTimeIncrement; Time < AnimationSequence->GetPlayLength(); Time+=AnimationTimeIncrement)
		{
			FVector BoneLocation = GetBoneLocation(AnimationSequence, ReferenceSkeleton, Bone.Value, Time);
			bool goingDown = BoneLocation.Z < PreviousBoneLocation.Z-FootstepSpeedThreshold;

			if(BoneLocation.Z > NewFootstepTrack.HighestBonePosition) NewFootstepTrack.HighestBonePosition = BoneLocation.Z;
			if(BoneLocation.Z < NewFootstepTrack.LowestBonePosition) NewFootstepTrack.LowestBonePosition = BoneLocation.Z;
			
			if(!goingDown && wasGoingDown) //Footstep!
			{
				const float FootSpeed = ((BoneLocation - PreviousBoneLocation)/FootstepSpeedCalculationWindow).Size();
				FFootstepAudioData& NewFootstep = NewFootstepTrack.Footsteps.AddDefaulted_GetRef();
				NewFootstep.BoneName = Bone.Key;
				NewFootstep.FootstepTime = Time;
				NewFootstep.FootstepSpeed = FootSpeed;

				//Values for filtering
				if(FootSpeed < NewFootstepTrack.SlowestBoneSpeed || NewFootstepTrack.SlowestBoneSpeed == -1.0f) NewFootstepTrack.SlowestBoneSpeed = FootSpeed;
				if(FootSpeed > NewFootstepTrack.FastestBoneSpeed) NewFootstepTrack.FastestBoneSpeed = FootSpeed;
			}

			wasGoingDown = goingDown;
			PreviousBoneLocation = BoneLocation;
		}
	}
	
}

int UAudioAnimationToolsWidget::GetAnimationTrackIndex(const int SkeletonBoneIndex, const UAnimSequence* AnimSequence)
{
	const TArray<FTrackToSkeletonMap>& TrackToSkeletonMaps = AnimSequence->GetCompressedTrackToSkeletonMapTable();
	if(SkeletonBoneIndex != INDEX_NONE)
	{
		for(int TrackIndex = 0; TrackIndex < TrackToSkeletonMaps.Num(); TrackIndex++)
		{
			if(TrackToSkeletonMaps[TrackIndex].BoneTreeIndex == SkeletonBoneIndex)
			{
				return TrackIndex;
			}
		}
	}

	return INDEX_NONE;
}

FVector UAudioAnimationToolsWidget::GetBoneLocation(const UAnimSequence* AnimSequence,
	const FReferenceSkeleton& ReferenceSkeleton, const int BoneIndex, const float Time)
{
	FTransform RootTransform;
	GetWorldToRootTransform(AnimSequence, Time, RootTransform);

	FTransform FootTransform;
	GetRootToBoneTransform(AnimSequence, ReferenceSkeleton, BoneIndex, Time, FootTransform);

	return RootTransform.TransformPosition(FootTransform.GetLocation());
}

void UAudioAnimationToolsWidget::GetWorldToRootTransform(const UAnimSequence* AnimSequence, const double Time,
                                                         FTransform& OutTransform)
{
	const FSkeletonPoseBoneIndex RootBoneIndex = FSkeletonPoseBoneIndex(0);
	if(RootBoneIndex != INDEX_NONE)
	{
		AnimSequence->GetBoneTransform(OutTransform, RootBoneIndex, Time, false);
	}
}

void UAudioAnimationToolsWidget::GetRootToBoneTransform(const UAnimSequence* AnimSequence, const FReferenceSkeleton& ReferenceSkeleton, const int BoneIndex, const double Time,
	FTransform& OutTransform)
{
	OutTransform = FTransform::Identity;

	int CurrentBoneIndex = BoneIndex;
	int NextBoneIndex = ReferenceSkeleton.GetParentIndex(CurrentBoneIndex);

	while(NextBoneIndex != INDEX_NONE)
	{
		FTransform ParentTransform;
		const FSkeletonPoseBoneIndex BoneIndex = FSkeletonPoseBoneIndex(CurrentBoneIndex);
		if(BoneIndex == INDEX_NONE)
		{
			ParentTransform = ReferenceSkeleton.GetRefBonePose()[CurrentBoneIndex];
		}
		else
		{
			AnimSequence->GetBoneTransform(ParentTransform, BoneIndex, Time, false);
		}

		OutTransform = OutTransform * ParentTransform;
		CurrentBoneIndex = NextBoneIndex;
		NextBoneIndex = ReferenceSkeleton.GetParentIndex(CurrentBoneIndex);
	}
	
}
