// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BuildSystemDataManager.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

#define TRY_LOAD_AND_TELL_SUCCESS(Asset, Type)\
	Asset = Cast<Type>(Config->Asset.LoadSynchronous());\
	if (Asset) \
	{ \
		UE_LOG(LogTemp, Log, TEXT("Asset chargée : %s"), *Asset->GetName()); \
	} \
	else \
	{ \
		UE_LOG(LogTemp, Error, TEXT("Impossible de charger l'asset : %s."), TEXT(#Asset)); \
	}



UDataTable* UBuildSystemDataManager::GetBuildableDataTable() const
{
	return BuildableDataTable;
}

UMaterialInterface* UBuildSystemDataManager::GetMaterialCanPlace() const
{
	return MaterialCanPlace;
}


UMaterialInterface* UBuildSystemDataManager::GetMaterialCanNotPlace() const
{
	return MaterialCanNotPlace;
}

void UBuildSystemDataManager::Initialize(FSubsystemCollectionBase& Collection)
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
		TRY_LOAD_AND_TELL_SUCCESS(BuildableDataTable, UDataTable);
		TRY_LOAD_AND_TELL_SUCCESS(MaterialCanPlace, UMaterialInterface);
		TRY_LOAD_AND_TELL_SUCCESS(MaterialCanNotPlace, UMaterialInterface);
	}

}
