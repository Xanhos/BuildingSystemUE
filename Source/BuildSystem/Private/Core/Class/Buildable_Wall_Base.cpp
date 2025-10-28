// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Class/Buildable_Wall_Base.h"


void ABuildable_Wall_Base::GenerateSockets()
{
	Super::GenerateSockets();

	FVector Extend = BuildableMesh->Bounds.BoxExtent;
	const FRotator& ZeroRota = FRotator::ZeroRotator;
	using Socket = ESocketConnectionType;
    

	AddSocket({Extend.X, 0, 0}, ZeroRota, Socket::Horizontal, {Socket::Horizontal});
	AddSocket({-Extend.X, 0, 0}, ZeroRota, Socket::Horizontal, {Socket::Horizontal}); 
	AddSocket({0, 0, -Extend.Z}, ZeroRota, Socket::Vertical, {Socket::Ground});  
	AddSocket({0, 0, Extend.Z}, ZeroRota, Socket::Vertical, {Socket::Vertical}); 
}