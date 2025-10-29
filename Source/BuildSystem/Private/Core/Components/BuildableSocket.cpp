// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Components/BuildableSocket.h"


// Sets default values for this component's properties
UBuildableSocket::UBuildableSocket()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBuildableSocket::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBuildableSocket::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UBuildableSocket::IsSocketCompatible(UBuildableSocket* Other)
{
	if (bIsOccupied || Other->bIsOccupied)
		return false;
	
	if (Other->Type == ESocketConnectionType::Any || Type == ESocketConnectionType::Any)
		return true;

	return Other->AcceptedType.Contains(Type) && AcceptedType.Contains(Other->Type);	
}

bool UBuildableSocket::LockSocket(UBuildableSocket* Other)
{
	if (!IsSocketCompatible(Other))
		return false;

	Other->bIsOccupied = true;
	Other->OccupiedSocket = this;

	bIsOccupied = true;
	OccupiedSocket = Other;
	return true;
}

void UBuildableSocket::ReleaseSocket(UBuildableSocket* Other)
{
	if (Other)
	{
		Other->bIsOccupied = false;
		Other->OccupiedSocket = nullptr;
	}

	bIsOccupied = false;
	OccupiedSocket = nullptr; 
}

