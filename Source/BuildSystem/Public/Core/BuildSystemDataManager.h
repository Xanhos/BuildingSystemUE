// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "Materials/MaterialInterface.h"
#include "BuildSystemDataManager.generated.h"

/**
 *
 */
UCLASS()
class BUILDSYSTEM_API UBuildSystemDataManager : public UEngineSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Data")
	UDataTable* GetBuildableDataTable() const;
	UFUNCTION(BlueprintCallable, Category = "Data")
	UMaterialInterface* GetMaterialCanPlace() const;
	UFUNCTION(BlueprintCallable, Category = "Data")
	UMaterialInterface* GetMaterialCanNotPlace() const;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	UPROPERTY()
	TObjectPtr<UDataTable> BuildableDataTable;
	TObjectPtr<UMaterialInterface> MaterialCanPlace;
	TObjectPtr<UMaterialInterface> MaterialCanNotPlace;
};


UCLASS(BlueprintType)
class BUILDSYSTEM_API UDataTableConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TSoftObjectPtr<UDataTable> BuildableDataTable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TSoftObjectPtr<UMaterialInterface> MaterialCanPlace;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TSoftObjectPtr<UMaterialInterface> MaterialCanNotPlace;
};