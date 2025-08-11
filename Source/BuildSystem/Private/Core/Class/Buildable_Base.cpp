// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Class/Buildable_Base.h"
#include "BuildingSystemFL.h"

// Sets default values
ABuildable_Base::ABuildable_Base()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BuildableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildableMesh"));
	if (BuildableMesh)
	{
		RootComponent = BuildableMesh;
	}

	FCoreDelegates::OnBeginFrame.AddUObject(this, &ABuildable_Base::InitializeMesh);


}

// Called when the game starts or when spawned
void ABuildable_Base::BeginPlay()
{
	Super::BeginPlay();

}

void ABuildable_Base::InitializeMesh() const
{
	bool FoundMesh = false;
	UStaticMesh* Mesh = UBuildingSystemFL::FL_GetBuildableMeshByTag(Tag, FoundMesh);
	if (FoundMesh)
	{
		BuildableMesh->SetStaticMesh(Mesh);
	}
}

// Called every frame
void ABuildable_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FGameplayTag ABuildable_Base::IF_GetBuildableTag_Implementation() const
{
	return Tag;
}

TArray<UPrimitiveComponent*> ABuildable_Base::IF_GetSocket_Implementation() const
{
	return TArray<UPrimitiveComponent*>();
}

