// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticMeshEmitterSourceComponent.h"

#include "Components/BillboardComponent.h"

// Sets default values for this component's properties
UStaticMeshEmitterSourceComponent::UStaticMeshEmitterSourceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

#if WITH_EDITORONLY_DATA
	bVisualizeComponent = true;
#endif

	// ...
}


// Called when the game starts
void UStaticMeshEmitterSourceComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UStaticMeshEmitterSourceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UStaticMeshEmitterSourceComponent::OnRegister()
{
	Super::OnRegister();
#if WITH_EDITORONLY_DATA
	if(SpriteComponent)
	{
		SpriteComponent->SetSprite(EditorDebugSprite);
	}
#endif
	
}

