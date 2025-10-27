// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Class/Buildable_Foundation_Base.h"

FVector ABuildable_Foundation_Base::ProcessSnappingPos(EBuildableSocketType Type)
{
	Super::ProcessSnappingPos(Type);
	FVector NewLocation = GetActorLocation();
	
	switch (Type)
	{
	case EBuildableSocketType::Forward:
		NewLocation +=  FVector{BuildableMesh->Bounds.BoxExtent.X,0,-BuildableMesh->Bounds.BoxExtent.Z};
		break;
	case EBuildableSocketType::Backward:
		NewLocation +=  FVector{-BuildableMesh->Bounds.BoxExtent.X,0,-BuildableMesh->Bounds.BoxExtent.Z};
		break;
	case EBuildableSocketType::Right:
		NewLocation +=  FVector{0,BuildableMesh->Bounds.BoxExtent.X,-BuildableMesh->Bounds.BoxExtent.Z};
		break;
	case EBuildableSocketType::Left:
		NewLocation +=  FVector{0,-BuildableMesh->Bounds.BoxExtent.X,-BuildableMesh->Bounds.BoxExtent.Z};
		break;
	case EBuildableSocketType::Up:
		break;
	case EBuildableSocketType::Down:
		break;
	case EBuildableSocketType::COUNT:
		break;
	default: ;
	}
	return NewLocation;
}
