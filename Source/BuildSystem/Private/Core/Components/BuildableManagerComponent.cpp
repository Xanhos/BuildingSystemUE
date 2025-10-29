// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Components/BuildableManagerComponent.h"

#include <filesystem>
#include <winsock2.h>

#include "BuildingSystemFL.h" 
#include "Camera/CameraComponent.h"
#include "Core/Class/Buildable_Base.h"
#include "GameFramework/Actor.h"
#include "Engine/TargetPoint.h"

DEFINE_LOG_CATEGORY(LogBuildableComponent);


// Sets default values for this component's properties
UBuildableManagerComponent::UBuildableManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bIsBuildModeActive = false;
	CurrentBuildPreviewIndex = 0;
	BuildPreviewActor = nullptr;

	// ...
}


// Called when the game starts
void UBuildableManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	UpdateAllBuildableTags();
	TryPrintAllBuildableTags();
	// ...

}

void UBuildableManagerComponent::UpdateAllBuildableTags()
{
	BuildableTags = UBuildingSystemFL::FL_GetAllBuildableTags();
}

void UBuildableManagerComponent::DeInitializeBuildPreview()
{
	if (BuildPreviewActor)
	{ 
		GetWorld()->GetTimerManager().ClearTimer(BuildPreviewTraceTimer);
		BuildPreviewTraceTimer.Invalidate();
		BuildPreviewActor->Destroy();
		BuildPreviewActor = nullptr;
	}
}

void UBuildableManagerComponent::InitializeBuildPreview()
{
	AActor* Owner = GetOwner();

	if (!BuildPreviewActor)
	{
		bool bHasFoundRow = false;
		FBuildableData Data = GetBuildableDataAtIndex(CurrentBuildPreviewIndex, bHasFoundRow);
		if (bHasFoundRow)
		{
			BuildPreviewActor = GetWorld()->SpawnActor<ABuildable_Base>(Data.Class);
		}
		 
		if (BuildPreviewActor)
		{
			BuildPreviewActor->SetIsPreviewBuild(true);
			BuildPreviewTrace(); 
			BuildPreviewActor->AttachToComponent(Owner->GetRootComponent(),
				FAttachmentTransformRules::KeepWorldTransform);
			ActivateBuildPreviewTrace();
		}
	}
}

FBuildableData UBuildableManagerComponent::GetBuildableDataAtIndex(int Index, bool& HasFoundRow) const
{
	FBuildableData Data = {};

	if (!BuildableTags.GetGameplayTagArray().IsValidIndex(Index))
	{
		UE_LOG(LogBuildableComponent, Error, TEXT("Index %d is out of bounds !"), Index);
		return Data;
	}

	Data = UBuildingSystemFL::FL_GetBuildableDataByTag(BuildableTags.GetGameplayTagArray()[Index], HasFoundRow);
	if (HasFoundRow)
	{
		return Data;
	}

	UE_LOG(LogBuildableComponent, Error, TEXT("Couldn't find the row at the index %d !"), Index);
	return Data;
}

void UBuildableManagerComponent::BuildPreviewTrace()
{
	bool bFoundRow = false;
	FBuildableData Data = GetBuildableDataAtIndex(CurrentBuildPreviewIndex, bFoundRow);
	if (!bFoundRow)
	{
		return;
	}


	FVector Start = GetOwner()->GetActorLocation();           
	FVector ForwardVector = CameraComponent->GetForwardVector(); 
	FVector End = Start + (ForwardVector * BuildPreviewTraceDistance);  

	FHitResult HitResult;                         
	FCollisionQueryParams Params;
	Params.bTraceComplex = false;
	Params.AddIgnoredActor(GetOwner());


	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult,
		Start,
		End,
		UEngineTypes::ConvertToCollisionChannel(Data.TraceChannel),
		Params);

	if (bEnableDebug)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.f, 1.f);
	}

	SnappingSocketPair={};
	FTransform LocationTransform{};
	if (bHit)
	{
		auto SetupTransformForOverlapPreview = [this](UBuildableSocket* Socket, UBuildableSocket* OtherSocket){
			FTransform NewTransform = BuildPreviewActor->GetSnapTransform( Socket,OtherSocket);
			SetBuildPreviewTransform(NewTransform);
			bool b=  !IsOverlappingBuildPreview();
			return b;
		};
		IBuildable* Buildable = Cast<IBuildable>(HitResult.GetActor());
		 
		
		if (Buildable && bSnappingIsActive)
		{
			UBuildableSocket* Socket = nullptr;
			UBuildableSocket* OutSocket = nullptr;
			if(BuildPreviewActor->TrySnapToClosestBuildableSocket(Cast<ABuildable_Base>(HitResult.GetActor()),Socket,OutSocket,SetupTransformForOverlapPreview))
			{
				SnappingSocketPair = {Socket, OutSocket};
				LocationTransform = BuildPreviewActor->GetSnapTransform( Socket,OutSocket);
			}
			
		}
		else
		{
			LocationTransform.SetLocation(HitResult.Location); 
		}
	}
	else
	{
		LocationTransform.SetLocation(HitResult.TraceEnd);	
			
	}
	SetBuildPreviewTransform(LocationTransform);

	bool bCanPlace = bHit && !IsOverlappingBuildPreview();
	
	UpdateBuildPreviewValidity(bCanPlace ? EBuildPreviewStatus::CanPlace : EBuildPreviewStatus::CanNotPlace); 
}


void UBuildableManagerComponent::ActivateBuildPreviewTrace()
{
	GetWorld()->GetTimerManager().SetTimer(
		BuildPreviewTraceTimer,
		this,
		&UBuildableManagerComponent::OnBuildPreviewTraceTick,
		BuildPreviewTraceTickRate,
		true);
}

void UBuildableManagerComponent::SetBuildPreviewTransform(const FTransform& Transform)
{
	BuildPreviewTransform = Transform;
	if (BuildPreviewActor)
	{
		BuildPreviewActor->SetActorTransform(BuildPreviewTransform);
	}
}

void UBuildableManagerComponent::OnBuildPreviewTraceTick()
{
	BuildPreviewTrace(); 
}

void UBuildableManagerComponent::UpdateBuildPreviewValidity(EBuildPreviewStatus PreviewStatus)
{
	if (!BuildPreviewActor)
	{
		return;
	}

	BuildStatus = PreviewStatus;
	UStaticMeshComponent* MeshComponent = IBuildable::Execute_IF_GetStaticMesh(BuildPreviewActor); 
	for (size_t i = 0; i < MeshComponent->GetNumMaterials(); i++)
	{
		UMaterialInterface* Material = UBuildingSystemFL::FL_GetPreviewMaterial(PreviewStatus);
		MeshComponent->SetMaterial(i, Material);
	}
}

void UBuildableManagerComponent::UpdateCurrentBuildPreviewIndex(EBuildPreviewUpdateMethod Method)
{
	switch (Method)
	{
	case EBuildPreviewUpdateMethod::Increment:
		if (CurrentBuildPreviewIndex + 1 > GetAllBuildableTagsLength())
		{
			UpdateCurrentBuildPreviewIndex(EBuildPreviewUpdateMethod::First);
			return;
		}
		CurrentBuildPreviewIndex++;
		break;
	case EBuildPreviewUpdateMethod::Decrement:
		if (CurrentBuildPreviewIndex - 1 < 0)
		{
			UpdateCurrentBuildPreviewIndex(EBuildPreviewUpdateMethod::Last);
			return;
		}
		CurrentBuildPreviewIndex--;
		break;
	case EBuildPreviewUpdateMethod::First:
		CurrentBuildPreviewIndex = 0;
		break;
	case EBuildPreviewUpdateMethod::Last:
		CurrentBuildPreviewIndex = GetAllBuildableTagsLength();
		break;
	default:
		break;
	}
	UpdateBuildPreviewMeshByIndex(CurrentBuildPreviewIndex);
}

void UBuildableManagerComponent::TryPlaceBuildableByIndex(int Index)
{
	if (BuildStatus != EBuildPreviewStatus::CanPlace)
	{
		return;
	}
	if (Index == -1)
	{
		Index = CurrentBuildPreviewIndex;
	}

	bool bHasFoundRow = false;
	FBuildableData Data = GetBuildableDataAtIndex(Index, bHasFoundRow);
	if (bHasFoundRow)
	{
		SpawnBuild(Data.Class);
	}
}

int UBuildableManagerComponent::GetAllBuildableTagsLength() const
{
	return BuildableTags.GetGameplayTagArray().Num() - 1;
}

void UBuildableManagerComponent::UpdateBuildPreviewMeshByIndex(int Index)
{
	bool bHasFoundRow = false;
	FBuildableData Data = GetBuildableDataAtIndex(Index, bHasFoundRow);
	if (bHasFoundRow)
	{
		DeInitializeBuildPreview();
		InitializeBuildPreview();
	}
}

void UBuildableManagerComponent::SpawnBuild(TSubclassOf<AActor> ActorToSpawn)
{ 
	ABuildable_Base* Buildable = GetWorld()->SpawnActor<ABuildable_Base>(ActorToSpawn, BuildPreviewTransform);
}

bool UBuildableManagerComponent::IsOverlappingBuildPreview()
{
	TArray<AActor*> OverlapActors;
	BuildPreviewActor->GetOverlappingActors(OverlapActors);
	return !OverlapActors.IsEmpty();
}


void UBuildableManagerComponent::InitializeBuildableManagerComponent(UCameraComponent* Camera)
{
	CameraComponent = Camera;
}

// Called every frame
void UBuildableManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBuildableManagerComponent::TryPrintAllBuildableTags() const
{
	if (!bEnableDebug)
	{
		return;
	}
	if (!GEngine)
	{
		UE_LOG(LogBuildableComponent, Error, TEXT("GEngine not ready yet !"));
		return;
	}
	for (FGameplayTag Tag : BuildableTags)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("%s"), *Tag.GetTagName().ToString()));
	}
}

void UBuildableManagerComponent::ActivateBuildMode()
{
	bIsBuildModeActive = true;
	InitializeBuildPreview();
}

void UBuildableManagerComponent::DeActivateBuildMode()
{
	bIsBuildModeActive = false;
	DeInitializeBuildPreview();
}

void UBuildableManagerComponent::ToggleBuildMode()
{
	bIsBuildModeActive = !bIsBuildModeActive;
	if (bIsBuildModeActive)
	{
		ActivateBuildMode();
	}
	else
	{
		DeActivateBuildMode();
	}
}

void UBuildableManagerComponent::ToggleSnapping()
{
	bSnappingIsActive = !bSnappingIsActive;
}
