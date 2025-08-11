// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/DataTableManager.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

UDataTable* UDataTableManager::GetBuildableDataTable() const
{
	return BuildableDataTable;
}

void UDataTableManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);


	static const FString ConfigClassPath = TEXT("/Game/Core/Data/BP_DataTableConfig.BP_DataTableConfig_C");


	if (UClass* BPClass = LoadClass<UDataTableConfig>(nullptr, *ConfigClassPath))
	{
		UDataTableConfig* Config = NewObject<UDataTableConfig>(this, BPClass);
		if (!Config)
		{
			return;
		}
		BuildableDataTable = Cast<UDataTable>(Config->BuildableDataTable.LoadSynchronous());
	}

	if (BuildableDataTable)
	{
		UE_LOG(LogTemp, Log, TEXT("DataTable chargée : %s"), *BuildableDataTable->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Impossible de charger la DataTable."));
	}
}
