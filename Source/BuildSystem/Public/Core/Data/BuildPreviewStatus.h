// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildPreviewStatus.generated.h"
/**
 *
 */
UENUM(BlueprintType, Blueprintable)
enum class EBuildPreviewStatus : uint8
{
	NONE,
	CanPlace,
	CanNotPlace
};

UENUM(BlueprintType, Blueprintable)
enum class EBuildPreviewUpdateMethod : uint8
{
	NONE,
	Increment,
	Decrement,
	First,
	Last
};
