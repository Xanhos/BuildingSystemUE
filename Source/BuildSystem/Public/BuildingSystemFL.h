// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Core/Data/BuildableData.h"
#include "Core/Data/BuildPreviewStatus.h"
#include "BuildingSystemFL.generated.h"




UCLASS()
class BUILDSYSTEM_API UBuildingSystemFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	static UDataTable* GetDataTable();
	static constexpr const TCHAR* DataTablePath = TEXT("/Game/Core/Data/DT_BuildableDefinition.DT_BuildableDefinition");
public:
	UFUNCTION(BlueprintCallable)
	static FBuildableData FL_GetBuildableDataByTag(FGameplayTag Tag, bool& FoundData);

	UFUNCTION(BlueprintCallable)
	static UStaticMesh* FL_GetBuildableMeshByTag(FGameplayTag Tag, bool& FoundMesh);

	UFUNCTION(BlueprintCallable)
	static FGameplayTagContainer FL_GetAllBuildableTags();

	UFUNCTION(BlueprintCallable)
	static UMaterialInterface* FL_GetPreviewMaterial(EBuildPreviewStatus PreviewStatus);

};
