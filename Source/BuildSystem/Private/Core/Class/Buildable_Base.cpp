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


	GenerateSockets();

    UBoxComponent* BoxComponent = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), TEXT("SnapBox"));
	BoxComponent->RegisterComponent();
	BoxComponent->AttachToComponent(BuildableMesh, FAttachmentTransformRules::KeepRelativeTransform);
	BoxComponent->SetBoxExtent(BuildableMesh->Bounds.BoxExtent);
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionResponseToChannels(BuildableMesh->GetCollisionResponseToChannels());
	
}

void ABuildable_Base::AddSocket(const FVector& Location, const FRotator& Rotation,
	ESocketConnectionType ConnectionType, const TArray<ESocketConnectionType>& AcceptedConnection, FVector Scale)
{
	FBuildingSocket BuildingSocket;
	BuildingSocket.Transform = FTransform(Rotation,Location, Scale);
	BuildingSocket.Type = ConnectionType;
	BuildingSocket.AcceptedTypes = AcceptedConnection;
	BuildingSocket.Index = SocketArray.Num();
	SocketArray.Add(BuildingSocket);
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

void ABuildable_Base::GenerateSockets()
{
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

TArray<FBuildingSocket> ABuildable_Base::IF_GetSocket_Implementation(ESocketConnectionType Type) const
{
	TArray<FBuildingSocket> SocketsOutput;
	Algo::CopyIf(SocketArray, SocketsOutput, [Type](const FBuildingSocket& Socket)
	{
		return Socket.AcceptedTypes.Contains(Type);
	});

	return SocketsOutput;
}

TArray<FBuildingSocket> ABuildable_Base::IF_GetAvailableSockets_Implementation()
{
	TArray<FBuildingSocket> SocketsOutput;
	Algo::CopyIf(SocketArray, SocketsOutput, [](const FBuildingSocket& Socket)
	{
		return !Socket.bIsOccupied;
	});

	return SocketsOutput;
}

UStaticMeshComponent* ABuildable_Base::IF_GetStaticMesh_Implementation() const
{
	return BuildableMesh;
}

void ABuildable_Base::IF_SnapToSocket_Implementation(FBuildingSocket Socket)
{
	ProcessSnappingPos(Socket);
}

FVector ABuildable_Base::ProcessSnappingPos(FBuildingSocket Socket)
{
	return GetActorLocation();
}
 
