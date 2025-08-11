// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Core/Interfaces/IBuildable.h"
#include "GameFramework/Actor.h"
#include "Buildable_Base.generated.h"

UCLASS(Abstract)
class BUILDSYSTEM_API ABuildable_Base : public AActor, public IBuildable
{
	GENERATED_BODY()

public:
	ABuildable_Base();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BuildableMesh;

	void InitializeMesh() const;
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag Tag;

	virtual void Tick(float DeltaTime) override;

	FGameplayTag IF_GetBuildableTag_Implementation() const override;

	TArray<UPrimitiveComponent*> IF_GetSocket_Implementation() const override;
};
