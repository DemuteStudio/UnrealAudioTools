// Fill out your copyright notice in the Description page of Project Settings.


#include "GroupedAudioComponent.h"

// Sets default values for this component's properties
UGroupedAudioComponent::UGroupedAudioComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGroupedAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGroupedAudioComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(isOn && !turnOn)
	{
		isOn = turnOn;
		TurnOffComponent();
	}
	else if(!isOn && turnOn)
	{
		isOn = turnOn;
		TurnOnComponent();
	}
	
	// ...
}

void UGroupedAudioComponent::TurnOnComponent_Implementation()
{
}

void UGroupedAudioComponent::TurnOffComponent_Implementation()
{
}


