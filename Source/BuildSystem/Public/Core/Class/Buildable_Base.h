// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/BoxComponent.h"
#include "Core/Interfaces/IBuildable.h"
#include "Engine/TargetPoint.h"
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<FBuildableSocketStruct> SocketArray;
	
	void InitializeMesh() const;
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag Tag;

	virtual void Tick(float DeltaTime) override;

	virtual FGameplayTag IF_GetBuildableTag_Implementation() const override;

	virtual TArray<FBuildableSocketStruct> IF_GetSocket_Implementation() const override;

	virtual UStaticMeshComponent* IF_GetStaticMesh_Implementation() const override;

	virtual void IF_SnapToSocket_Implementation(FBuildableSocketStruct Socket) override;
	
	virtual FVector ProcessSnappingPos(EBuildableSocketType Type);
};
