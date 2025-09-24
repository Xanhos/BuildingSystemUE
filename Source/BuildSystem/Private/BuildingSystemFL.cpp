// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingSystemFL.h"
#include "Core/BuildSystemDataManager.h"
#include "Core/Components/BuildableManagerComponent.h"

UDataTable* UBuildingSystemFL::GetDataTable()
{
	static UDataTable* CachedDataTable = nullptr;
	if (!GEngine)
	{
		UE_LOG(LogTemp, Error, TEXT("GEngine not ready yet !"));
		return nullptr;
	}

	UBuildSystemDataManager* DataTableManager = GEngine->GetEngineSubsystem<UBuildSystemDataManager>();
	if (!DataTableManager)
	{
		UE_LOG(LogTemp, Error, TEXT("DataTableManager not ready yet !"));
		return nullptr;
	}

	if (!CachedDataTable)
	{
		CachedDataTable = DataTableManager->GetBuildableDataTable();
	}

	return CachedDataTable;
}

FBuildableData UBuildingSystemFL::FL_GetBuildableDataByTag(FGameplayTag Tag, bool& FoundData)
{
	UDataTable* DataTable = GetDataTable();
	if (!DataTable)
	{
		FoundData = false;
		return FBuildableData();
	}

	FBuildableData* Row = DataTable->FindRow<FBuildableData>(Tag.GetTagName(), "BuildingSystem");
	if (!Row)
	{
		FoundData = false;
		return FBuildableData();
	}

	FoundData = true;
	return *Row;
}

UStaticMesh* UBuildingSystemFL::FL_GetBuildableMeshByTag(FGameplayTag Tag, bool& FoundMesh)
{
	if (Tag == FGameplayTag())
	{
		UE_LOG(LogBuildableComponent, Warning, TEXT("Tag was null"));
		return nullptr;
	}

	UDataTable* DataTable = GetDataTable();
	if (!DataTable)
	{
		FoundMesh = false;
		return nullptr;
	}

	UE_LOG(LogTemp, Warning, TEXT("Searching for tag: %s"), *Tag.ToString());
	FBuildableData* Row = DataTable->FindRow<FBuildableData>(Tag.GetTagName(), "BuildingSystem_GetBuildableMeshByTag");
	if (!Row || !Row->Mesh)
	{
		FoundMesh = false;
		return nullptr;
	}

	FoundMesh = true;
	return Row->Mesh;
}

FGameplayTagContainer UBuildingSystemFL::FL_GetAllBuildableTags()
{
	UDataTable* DataTable = GetDataTable();
	FGameplayTagContainer BuildableContainer;

	if (!DataTable)
	{
		return BuildableContainer;
	}


	for (FName Name : DataTable->GetRowNames())
	{
		if (FBuildableData* Row = DataTable->FindRow<FBuildableData>(Name, "BuildingSystem_GetAllBuildableTags"))
		{
			BuildableContainer.AddTag(Row->Tag);
		}
	}

	return BuildableContainer;
}

UMaterialInterface* UBuildingSystemFL::FL_GetPreviewMaterial(EBuildPreviewStatus PreviewStatus)
{
	if (UBuildSystemDataManager* BuildSystemDataManager = GEngine->GetEngineSubsystem<UBuildSystemDataManager>())
	{
		switch (PreviewStatus)
		{
		case EBuildPreviewStatus::CanPlace:
			return BuildSystemDataManager->GetMaterialCanPlace();
		case EBuildPreviewStatus::CanNotPlace:
			return BuildSystemDataManager->GetMaterialCanNotPlace();
		default:
			return nullptr;
		}
	}
	return nullptr;
}
