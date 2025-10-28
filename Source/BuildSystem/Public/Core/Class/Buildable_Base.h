// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/BoxComponent.h"
#include "Core/Interfaces/IBuildable.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Actor.h"
#include "Buildable_Base.generated.h"


UCLASS(Abstract)
class BUILDSYSTEM_API ABuildable_Base : public AActor, public IBuildable
{
	GENERATED_BODY()

public:
	ABuildable_Base();

protected:
	virtual void BeginPlay() override;

	void AddSocket(const FVector& Location,const FRotator& Rotation, ESocketConnectionType ConnectionType,const TArray<ESocketConnectionType>& AcceptedConnection, FVector Scale = FVector{1,1,1});
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BuildableMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<FBuildingSocket> SocketArray;
	
	void InitializeMesh() const;

	virtual void GenerateSockets();
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag Tag;

	virtual void Tick(float DeltaTime) override;

	virtual FGameplayTag IF_GetBuildableTag_Implementation() const override;

	virtual TArray<FBuildingSocket> IF_GetSocket_Implementation(ESocketConnectionType Type) const override;

	virtual TArray<FBuildingSocket> IF_GetAvailableSockets_Implementation() override;

	virtual UStaticMeshComponent* IF_GetStaticMesh_Implementation() const override;

	virtual void IF_SnapToSocket_Implementation(FBuildingSocket Socket) override;
	
	virtual FVector ProcessSnappingPos(FBuildingSocket Socket);
};
