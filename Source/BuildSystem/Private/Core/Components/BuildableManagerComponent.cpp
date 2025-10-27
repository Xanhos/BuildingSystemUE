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
	BuildPreviewComponent = nullptr;

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

void UBuildableManagerComponent::InitializeBuildPreview()
{
	AActor* Owner = GetOwner();

	if (!BuildPreviewComponent)
	{
		BuildPreviewComponent = Cast<UStaticMeshComponent>(Owner->AddComponentByClass(UStaticMeshComponent::StaticClass(), false, {}, true));
		if (BuildPreviewComponent)
		{
			BuildPreviewComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			BuildPreviewComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
			BuildPreviewTrace(); 
			BuildPreviewComponent->RegisterComponent();
			BuildPreviewComponent->AttachToComponent(Owner->GetRootComponent(),
				FAttachmentTransformRules::KeepWorldTransform);
		}
	}
	UpdateBuildPreviewMeshByIndex(CurrentBuildPreviewIndex);
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

	FTransform LocationTransform{};
	if (bHit)
	{
		IBuildable* Buildable = Cast<IBuildable>(HitResult.GetActor());
		 
		
		if (Buildable && bSnappingIsActive)
		{
			auto SetupTransformForOverlapPreview = [this](const FVector& Pos)
			{
				FTransform NewTransform = BuildPreviewTransform;
				NewTransform.SetLocation(Pos);
				SetBuildPreviewTransform(NewTransform);
			};
			
			FVector NewPos = HitResult.Location;
			for (FBuildableSocketStruct& Socket : Buildable->Execute_IF_GetSocket(HitResult.GetActor()))
			{ 
				if (NewPos == HitResult.Location)
				{
					SetupTransformForOverlapPreview(Socket.Position);
					if (!IsOverlappingBuildPreview())
					{
						NewPos = Socket.Position;
						SnappingSocket = Socket;
					}
					continue;
				}
				
				if (FVector::Distance(Socket.Position, HitResult.Location) < FVector::Distance(HitResult.Location, NewPos))
				{
					SetupTransformForOverlapPreview(Socket.Position);
					if (!IsOverlappingBuildPreview())
					{
						NewPos = Socket.Position;
						SnappingSocket = Socket;
					}					
				}
			}
			

			LocationTransform.SetLocation(NewPos);			
		}
		else
		{
			LocationTransform.SetLocation(HitResult.Location);
			SnappingSocket.Type = EBuildableSocketType::INVALID;
		}
	}
	else
	{
		LocationTransform.SetLocation(HitResult.TraceEnd);	
		SnappingSocket.Type = EBuildableSocketType::INVALID;	
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
	if (BuildPreviewComponent)
	{
		BuildPreviewTransform = Transform;
		BuildPreviewTransform.SetLocation(BuildPreviewTransform.GetLocation() + FVector(0, 0, BuildPreviewComponent->Bounds.BoxExtent.Z));
		BuildPreviewComponent->SetWorldTransform(BuildPreviewTransform);
	}
}

void UBuildableManagerComponent::OnBuildPreviewTraceTick()
{
	BuildPreviewTrace(); 
}

void UBuildableManagerComponent::UpdateBuildPreviewValidity(EBuildPreviewStatus PreviewStatus)
{
	if (!BuildPreviewComponent)
	{
		return;
	}

	BuildStatus = PreviewStatus;
	for (size_t i = 0; i < BuildPreviewComponent->GetNumMaterials(); i++)
	{
		UMaterialInterface* Material = UBuildingSystemFL::FL_GetPreviewMaterial(PreviewStatus);
		BuildPreviewComponent->SetMaterial(i, Material);
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
		BuildPreviewComponent->SetStaticMesh(Data.Mesh);
	}
}

void UBuildableManagerComponent::SpawnBuild(TSubclassOf<AActor> ActorToSpawn)
{ 
	ABuildable_Base* Buildable = GetWorld()->SpawnActor<ABuildable_Base>(ActorToSpawn, BuildPreviewTransform);
}

bool UBuildableManagerComponent::IsOverlappingBuildPreview()
{
	TArray<AActor*> OverlapActors;
	BuildPreviewComponent->GetOverlappingActors(OverlapActors);
	return !BuildPreviewComponent->GetOverlapInfos().IsEmpty();
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
	ActivateBuildPreviewTrace();
}

void UBuildableManagerComponent::DeActivateBuildMode()
{
	bIsBuildModeActive = false;
	GetWorld()->GetTimerManager().ClearTimer(BuildPreviewTraceTimer);
	BuildPreviewTraceTimer.Invalidate();
	BuildPreviewComponent->DestroyComponent();
	BuildPreviewComponent = nullptr;
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
