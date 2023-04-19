// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaticMeshEmitterSourceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DM_STATICMESHEMITTERS_API UStaticMeshEmitterSourceComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStaticMeshEmitterSourceComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnRegister() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Static Mesh Emitter Source Component")
	UTexture2D* EditorDebugSprite;
};
