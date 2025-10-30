// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/BoxComponent.h"
#include "Core/Components/BuildableSocket.h"
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
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BuildableMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> BuildableRoot;
	
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UBuildableSocket>> SocketArray;
	
	void InitializeMesh() const;

	void GenerateSockets();

	virtual void OnGenerateSockets();

	bool GetClosestSocket(UBuildableSocket* OtherSocket,const float MaxDistance ,const FVector& AtLocation,UBuildableSocket*& OutSocket
		, const TFunction<bool(UBuildableSocket*, UBuildableSocket*)>& Predicate = [](auto f, auto d){return true;});
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag Tag;

	virtual void Tick(float DeltaTime) override;

	virtual FGameplayTag IF_GetBuildableTag_Implementation() const override;

	virtual TArray<UBuildableSocket*> IF_GetSocket_Implementation(ESocketConnectionType Type) const override;

	virtual TArray<UBuildableSocket*> IF_GetAvailableSockets_Implementation() override;

	virtual UStaticMeshComponent* IF_GetStaticMesh_Implementation() const override;

	virtual bool IF_GetClosestSocket_Implementation(UBuildableSocket* OtherSocket, float MaxDistance, UBuildableSocket*& OutSocket) override;
	
	virtual FVector ProcessSnappingPos(UBuildableSocket* Socket);

	void SetIsPreviewBuild(bool bIsPreview);

	void DrawDebugSocket();
 
	bool TrySnapToClosestBuildableSocket(ABuildable_Base* OtherBuilding, const FVector& AtLocation, UBuildableSocket*& OutSocket, UBuildableSocket*& OutOtherSocket,
		const TFunction<bool(UBuildableSocket*, UBuildableSocket*)>& Predicate = [](auto f, auto d){return true;});
 
	FVector GetSnapPosition(UBuildableSocket* Socket, UBuildableSocket* OtherSocket);
	
	// If you also need rotation alignment:
	FTransform GetSnapTransform(UBuildableSocket* Socket, UBuildableSocket* OtherSocket);
};
