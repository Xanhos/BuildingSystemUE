// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingSystemFL.h"
#include "Core/DataTableManager.h"

UDataTable* UBuildingSystemFL::GetDataTable()
{
	static UDataTable* CachedDataTable = nullptr;
	if (!GEngine)
	{
		UE_LOG(LogTemp, Error, TEXT("GEngine not ready yet !"));
		return nullptr;
	}

	UDataTableManager* DataTableManager = GEngine->GetEngineSubsystem<UDataTableManager>();
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
	UDataTable* DataTable = GetDataTable();
	if (!DataTable)
	{
		FoundMesh = false;
		return nullptr;
	}

	FBuildableData* Row = DataTable->FindRow<FBuildableData>(Tag.GetTagName(), "BuildingSystem");
	if (!Row || !Row->Mesh)
	{
		FoundMesh = false;
		return nullptr;
	}

	FoundMesh = true;
	return Row->Mesh;
}
