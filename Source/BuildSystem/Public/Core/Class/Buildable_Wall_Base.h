// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Class/Buildable_Base.h"
#include "Buildable_Wall_Base.generated.h"

/**
 *
 */
UCLASS(Abstract)
class BUILDSYSTEM_API ABuildable_Wall_Base : public ABuildable_Base
{
	GENERATED_BODY()
	virtual void GenerateSockets() override;
};
