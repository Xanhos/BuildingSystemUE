// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "DataTableManager.generated.h"

/**
 *
 */
UCLASS()
class BUILDSYSTEM_API UDataTableManager : public UEngineSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Data")
	UDataTable* GetBuildableDataTable() const;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	UPROPERTY()
	TObjectPtr<UDataTable> BuildableDataTable;
};


UCLASS(BlueprintType)
class BUILDSYSTEM_API UDataTableConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TSoftObjectPtr<UDataTable> BuildableDataTable;
};