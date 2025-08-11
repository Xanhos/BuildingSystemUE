// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "IBuildable.generated.h"

/**
 *
 */
UINTERFACE(Blueprintable)
class BUILDSYSTEM_API UBuildable : public UInterface
{
	GENERATED_BODY()
};


class BUILDSYSTEM_API IBuildable
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FGameplayTag IF_GetBuildableTag() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<UPrimitiveComponent*> IF_GetSocket() const;
};
