// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlotData.h"
#include "AttributeData.h"
#include "GameplayTagContainer.h"
#include "Core/Interfaces/IBuildable.h"
#include "StructUtils/InstancedStruct.h"
//#include "InstancedStruct.h"
#include "BuildableData.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct BUILDSYSTEM_API FBuildableData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag Tag = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Class = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ETraceTypeQuery> TraceChannel = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSlotData> Cost = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, float> Attributes = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInstancedStruct CustomData = {};

};
