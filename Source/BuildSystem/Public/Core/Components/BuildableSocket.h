// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BuildableSocket.generated.h"

UENUM(BlueprintType)
enum class ESocketConnectionType : uint8
{
	Ground,      
	Vertical,    
	Horizontal,  
	Ceiling,     
	Any          
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BUILDSYSTEM_API UBuildableSocket : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBuildableSocket();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	ESocketConnectionType Type;
	
	UPROPERTY(EditAnywhere)
	TArray<ESocketConnectionType> AcceptedType;

	UPROPERTY(BlueprintReadWrite)
	bool bIsOccupied;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UBuildableSocket> OccupiedSocket;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	bool IsSocketCompatible(UBuildableSocket* Other);

	bool LockSocket(UBuildableSocket* Other);

	void ReleaseSocket(UBuildableSocket* Other);

	const TArray<ESocketConnectionType>& GetAcceptedType() const { return AcceptedType; }

	bool IsOccupied() const { return bIsOccupied; }

	
};
