// Fill out your copyright notice in the Description page of Project Settings.


#include "AudioSourcesGroup.h"

#include "DrawDebugHelpers.h"
#include "GroupedAudioComponent.h"
#include "GroupedAudioSourcesManager.h"
#include "Kismet/GameplayStatics.h"

FAudioSourcesGroup::FAudioSourcesGroup(const FName& InName, const FAudioSourcesGroupSettings& InGroupSettings,
                                       FGroupedAudioSourcesManager* InOwningGroupManagerPtr)
	: Name(InName)
	  , GroupSettings(InGroupSettings)
	  , OwningGroupManagerPtr(InOwningGroupManagerPtr)
{
}

FAudioSourcesGroup::~FAudioSourcesGroup()
{
}

void FAudioSourcesGroup::Tick(float DeltaTime)
{
	//UE_LOG(LogTemp, Display, TEXT("I'm ticking! %s"), *Name.ToString());

	Clusters.Empty();
	//UE_LOG(LogTemp, Display, TEXT("GAP : New group tick %s"), *GetName().ToString());

	for (UGroupedAudioComponent* TrackedAudioComponent : TrackedAudioComponents)
	{
		//UE_LOG(LogTemp, Display, TEXT("GAP : New component calculation %s"), *TrackedAudioComponent->GetName());
		bool bClustered = false;
		FVector CompPosition = TrackedAudioComponent->GetComponentLocation();
		float DistanceSquared = (CompPosition - OwningGroupManagerPtr->CurrentListenerPosition).SizeSquared();

		//Don't cluster sounds closer than half the max attenuation distance.
		if(GroupSettings.MaxAttenuationDistance*GroupSettings.MaxAttenuationDistance*0.25f > DistanceSquared)
		{
			TrackedAudioComponent->turnOn = true;
			continue;
		}
		//UE_LOG(LogTemp, Display, TEXT("GAP : Component position is %s"), *CompPosition.ToString());
		//UE_LOG(LogTemp, Display, TEXT("GAP : Squared distance is %f"), DistanceSquared);

		//DEBUG
		DrawDebugLine(TrackedAudioComponent->GetWorld(), CompPosition, OwningGroupManagerPtr->CurrentListenerPosition, FColor::Red, false
			, -1, 0,2); 
		//UE_LOG(LogTemp, Display, TEXT("GAP : Checking for clusters. Currently %d to compare with"), Clusters.Num());
		for (UAudioCluster* Cluster : Clusters)
		{
			float DistanceToClusterSquared = (CompPosition - Cluster->ClusterLocation).SizeSquared();

			//UE_LOG(LogTemp, Display, TEXT("GAP : Squared distance to cluster is %f"), DistanceToClusterSquared);
			//UE_LOG(LogTemp, Display, TEXT("GAP : Squared distance of cluster to listener is %f"), Cluster->ClusterDistanceSquared);

			if (DistanceSquared > DistanceToClusterSquared || Cluster->ClusterDistanceSquared >
				DistanceToClusterSquared)
			{
				//UE_LOG(LogTemp, Display, TEXT("GAP : Clustering component"));
				//cluster this component
				if (Cluster->ClusterDistanceSquared > DistanceSquared)
				{
					//This component is the closest one
					Cluster->ChangeClusterRoot(TrackedAudioComponent, CompPosition, DistanceSquared);
					//UE_LOG(LogTemp, Display, TEXT("GAP : Component is taking cluster root"));
				}
				else
				{
					TrackedAudioComponent->turnOn = false;
					//UE_LOG(LogTemp, Display, TEXT("GAP : Component is turned off"));
				}

				bClustered = true;
			}
		}
		if (!bClustered)
		{
			//Component wasn't clustered to any existing cluster, create new one
			UAudioCluster* NewCluster = NewObject<UAudioCluster>();
			//UE_LOG(LogTemp, Display, TEXT("GAP : Created new cluster"));
			NewCluster->ChangeClusterRoot(TrackedAudioComponent, CompPosition, DistanceSquared);
			Clusters.Add(NewCluster);
		}
	}
}

bool FAudioSourcesGroup::DoesMatchSettings(const FAudioSourcesGroupSettings& InGroupSettings) const
{
	//TODO: implement this
	return true;
}

void FAudioSourcesGroup::ChangeGroupSettings(const FAudioSourcesGroupSettings& InNewSettings)
{
}

int FAudioSourcesGroup::GetActiveSourcesCount() const
{
	return TrackedAudioComponents.Num();
}

int FAudioSourcesGroup::GetActiveClustersCount() const
{
	return Clusters.Num();
}
