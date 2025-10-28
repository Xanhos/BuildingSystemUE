// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "IBuildable.generated.h"

/**
 *
 */

UENUM(BlueprintType)
enum class ESocketConnectionType : uint8
{
	Ground,      
	Vertical,    
	Horizontal,  
	Ceiling,     
	Any          
};

USTRUCT(BlueprintType)
struct FBuildingSocket
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FTransform Transform;

	UPROPERTY(EditAnywhere)
	ESocketConnectionType Type;
	
	UPROPERTY(EditAnywhere)
	TArray<ESocketConnectionType> AcceptedTypes;

	UPROPERTY(EditAnywhere)
	bool bIsOccupied = false;

	UPROPERTY(EditAnywhere)
	int Index = 0;

	bool operator==(const FBuildingSocket& other) const
	{
		if (other.Type == ESocketConnectionType::Any || Type == ESocketConnectionType::Any)
		{
			return true;
		}
     
		bool bAAcceptsB = other.AcceptedTypes.Contains(Type);
		bool bBAcceptsA = AcceptedTypes.Contains(other.Type);
    
		return bAAcceptsB && bBAcceptsA;
	}
};


UINTERFACE(Blueprintable)
class BUILDSYSTEM_API UBuildable : public UInterface
{
	GENERATED_BODY()
};


class BUILDSYSTEM_API IBuildable
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FGameplayTag IF_GetBuildableTag() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<FBuildingSocket> IF_GetSocket(ESocketConnectionType Type) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UStaticMeshComponent* IF_GetStaticMesh() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void IF_SnapToSocket(int Index,FBuildingSocket Socket);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<FBuildingSocket> IF_GetAvailableSockets();
 
};
