// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Core/Class/Buildable_Base.h"
#include "Core/Data/BuildableData.h"
#include "Core/Data/BuildPreviewStatus.h"
#include "BuildableManagerComponent.generated.h"

class UCameraComponent;
DECLARE_LOG_CATEGORY_EXTERN(LogBuildableComponent, Log, All);

DECLARE_DELEGATE(BuildableComponentDelegate)

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BUILDSYSTEM_API UBuildableManagerComponent : public UActorComponent
{
	GENERATED_BODY()
private: 
	TPair<UBuildableSocket*, UBuildableSocket*> SnappingSocketPair;
	
	FGameplayTagContainer BuildableTags;

	bool bIsBuildModeActive;

	FTransform BuildPreviewTransform;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	ABuildable_Base* BuildPreviewActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float BuildPreviewTraceTickRate = 0.01f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float BuildPreviewTraceDistance = 1200.f;

	EBuildPreviewStatus BuildStatus;

	int CurrentBuildPreviewIndex;

	UPROPERTY()
	UCameraComponent* CameraComponent;

	bool bSnappingIsActive = false;

public:
	// Sets default values for this component's properties
	UBuildableManagerComponent();


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bEnableDebug = false;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void UpdateAllBuildableTags();

	void DeInitializeBuildPreview();
	
	void InitializeBuildPreview();

	FBuildableData GetBuildableDataAtIndex(int Index, bool& HasFoundRow) const;

	void BuildPreviewTrace();

	void ActivateBuildPreviewTrace();

	void SetBuildPreviewTransform(const FTransform& PreviewTransform);

	FTimerHandle BuildPreviewTraceTimer;

	void OnBuildPreviewTraceTick();

	void UpdateBuildPreviewValidity(EBuildPreviewStatus PreviewStatus);

	int GetAllBuildableTagsLength() const;

	void UpdateBuildPreviewMeshByIndex(int Index);

	void SpawnBuild(TSubclassOf<AActor> ActorToSpawn);

	bool IsOverlappingBuildPreview();
public:
	void InitializeBuildableManagerComponent(UCameraComponent* Camera);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void TryPrintAllBuildableTags() const;

	UFUNCTION(BlueprintCallable)
	bool IsBuildModeActive() const { return bIsBuildModeActive; }

	UFUNCTION(BlueprintCallable)
	int GetCurrentBuildPreviewIndex() const { return CurrentBuildPreviewIndex; }

	UFUNCTION(BlueprintCallable)
	void ActivateBuildMode();

	UFUNCTION(BlueprintCallable)
	void DeActivateBuildMode();

	UFUNCTION(BlueprintCallable)
	void ToggleBuildMode();


	UFUNCTION(BlueprintCallable)
	void ToggleSnapping();

	UFUNCTION(BlueprintCallable)
	void UpdateCurrentBuildPreviewIndex(EBuildPreviewUpdateMethod Method);

	UFUNCTION(BlueprintCallable)
	void TryPlaceBuildableByIndex(int Index = -1);
};
