// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Class/Buildable_Base.h"
#include "BuildingSystemFL.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABuildable_Base::ABuildable_Base()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BuildableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildableMesh"));
	if (BuildableMesh)
	{
		RootComponent = BuildableMesh;
	}

	FCoreDelegates::OnBeginFrame.AddUObject(this, &ABuildable_Base::InitializeMesh);
}

// Called when the game starts or when spawned
void ABuildable_Base::BeginPlay()
{
	Super::BeginPlay();


	for (EBuildableSocketType i = EBuildableSocketType::Forward; i < EBuildableSocketType::COUNT; ++i)
	{
		FVector Socket;
		FVector Extent = BuildableMesh->Bounds.BoxExtent;
		switch (i)
		{
			case EBuildableSocketType::Forward:
			Socket = FVector{Extent.X *2  ,0,-Extent.Z} + GetActorLocation();
			break;
			case EBuildableSocketType::Backward:
			Socket= FVector{(Extent.X ) *2 * -1,0,-Extent.Z} + GetActorLocation();
			break;
			case EBuildableSocketType::Right:
			Socket= FVector{0,(Extent.X )*2  * -1,-Extent.Z} + GetActorLocation();
			break;
			case EBuildableSocketType::Left:
			Socket= FVector{0,Extent.X *2 ,-Extent.Z} + GetActorLocation();
			break;
			case EBuildableSocketType::Up:
			Socket= FVector{0,0,Extent.Z}  + GetActorLocation();
			break;
			case EBuildableSocketType::Down:
			Socket= FVector{0,0,-Extent.Z}  + GetActorLocation();
			break;
			
			default:
			continue;
			break;
		}
		SocketArray.Add({Socket, static_cast<EBuildableSocketType>(i)});
	}

    UBoxComponent* BoxComponent = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), TEXT("SnapBox"));
	BoxComponent->RegisterComponent();
	BoxComponent->AttachToComponent(BuildableMesh, FAttachmentTransformRules::KeepRelativeTransform);
	BoxComponent->SetBoxExtent(BuildableMesh->Bounds.BoxExtent);
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionResponseToChannels(BuildableMesh->GetCollisionResponseToChannels());
	
}

void ABuildable_Base::InitializeMesh() const
{
	FCoreDelegates::OnBeginFrame.RemoveAll(this);
	bool FoundMesh = false;
	UStaticMesh* Mesh = UBuildingSystemFL::FL_GetBuildableMeshByTag(Tag, FoundMesh);
	if (FoundMesh)
	{
		BuildableMesh->SetStaticMesh(Mesh);
	}

}

// Called every frame
void ABuildable_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FGameplayTag ABuildable_Base::IF_GetBuildableTag_Implementation() const
{
	return Tag;
}

TArray<FBuildableSocketStruct> ABuildable_Base::IF_GetSocket_Implementation() const
{
	return SocketArray;
}

UStaticMeshComponent* ABuildable_Base::IF_GetStaticMesh_Implementation() const
{
	return BuildableMesh;
}

void ABuildable_Base::IF_SnapToSocket_Implementation(FBuildableSocketStruct Socket)
{
	ProcessSnappingPos(Socket.Type);
}

FVector ABuildable_Base::ProcessSnappingPos(EBuildableSocketType Type)
{
	return GetActorLocation();
}
 
