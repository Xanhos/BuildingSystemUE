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
enum class EBuildableSocketType : uint8
{
	Forward = 0,
	Backward,
	Right,
	Left,
	Up,
	Down,
	COUNT,
	INVALID
	
};

inline EBuildableSocketType& operator++(EBuildableSocketType& Socket)
{ 
	Socket = static_cast<EBuildableSocketType>(static_cast<int>(Socket) + 1);
	return Socket;
}

USTRUCT(BlueprintType)
struct FBuildableSocketStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBuildableSocketType Type;
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
	TArray<FBuildableSocketStruct> IF_GetSocket() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UStaticMeshComponent* IF_GetStaticMesh() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void IF_SnapToSocket(FBuildableSocketStruct Socket);
};
