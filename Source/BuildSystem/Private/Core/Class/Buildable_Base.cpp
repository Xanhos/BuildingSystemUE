// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Class/Buildable_Base.h"
#include "BuildingSystemFL.h"
#include "Kismet/GameplayStatics.h"
#include <limits>

// Sets default values
ABuildable_Base::ABuildable_Base()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BuildableRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	if (BuildableRoot)
	{
		RootComponent = BuildableRoot;
	}
	
	BuildableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildableMesh"));
	if (BuildableMesh)
	{
		BuildableMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}

	FCoreDelegates::OnBeginFrame.AddUObject(this, &ABuildable_Base::InitializeMesh);
}

// Called when the game starts or when spawned
void ABuildable_Base::BeginPlay()
{
	Super::BeginPlay();

	BuildableMesh->SetRelativeLocation({0,0,BuildableMesh->Bounds.BoxExtent.Z});
	
	GenerateSockets();

	BoxComp = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), TEXT("SnapBox"));
	BoxComp->RegisterComponent();
	BoxComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	BoxComp->SetBoxExtent(BuildableMesh->Bounds.BoxExtent);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComp->SetCollisionResponseToChannels(BuildableMesh->GetCollisionResponseToChannels());
	
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
	TArray<UBuildableSocket*> Sockets;
	
	GetComponents<UBuildableSocket>(Sockets);
	SocketArray = Sockets;

	OnGenerateSockets();
	
	GEngine->AddOnScreenDebugMessage(0,1,FColor::Red, *FString::Printf(TEXT("%d"), SocketArray.Num()));
}

void ABuildable_Base::OnGenerateSockets()
{
}

// Called every frame
void ABuildable_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugSocket();

}

FGameplayTag ABuildable_Base::IF_GetBuildableTag_Implementation() const
{
	return Tag;
}

TArray<UBuildableSocket*> ABuildable_Base::IF_GetSocket_Implementation(ESocketConnectionType Type) const
{
	TArray<UBuildableSocket*> SocketsOutput;
	Algo::CopyIf(SocketArray, SocketsOutput, [&](const UBuildableSocket* Socket)
	{
		return Socket->GetAcceptedType().Contains(Type);
	});

	return SocketsOutput;
}

TArray<UBuildableSocket*> ABuildable_Base::IF_GetAvailableSockets_Implementation()
{
	TArray<UBuildableSocket*> SocketsOutput;
	Algo::CopyIf(SocketArray, SocketsOutput, [](const UBuildableSocket* Socket)
	{
		return !Socket->IsOccupied();
	});

	return SocketsOutput;
}

UStaticMeshComponent* ABuildable_Base::IF_GetStaticMesh_Implementation() const
{
	return BuildableMesh;
}

bool ABuildable_Base::IF_GetClosestSocket_Implementation(UBuildableSocket* OtherSocket, float MaxDistance,
	UBuildableSocket*& OutSocket)
{
	return GetClosestSocket(OtherSocket,MaxDistance,OtherSocket->GetComponentLocation(),OutSocket);
}
 

FVector ABuildable_Base::ProcessSnappingPos(UBuildableSocket* Socket)
{
	return GetActorLocation();
}

void ABuildable_Base::SetIsPreviewBuild(bool bIsPreview)
{
	if (bIsPreview)
	{
		BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
		BuildableMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		BuildableMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	}
	else
	{

	}
}

void ABuildable_Base::DrawDebugSocket()
{
	for (TObjectPtr<UBuildableSocket> BuildableSocket : SocketArray)
	{
		DrawDebugSphere(GetWorld(), BuildableSocket->GetComponentLocation(),5.f,25,FColor::Red);
	}
}

bool ABuildable_Base::GetClosestSocket(UBuildableSocket* OtherSocket,const float MaxDistance, const FVector& AtLocation, UBuildableSocket*& OutSocket,
	const TFunction<bool(UBuildableSocket*, UBuildableSocket*)>& Function)
{
	float BestDistance = MaxDistance;
	bool bHasFound = false;
	
	for (UBuildableSocket* Socket : IBuildable::Execute_IF_GetAvailableSockets(this))
	{
		if (!OtherSocket->IsSocketCompatible(Socket))
			continue;

		
		
		if (const float Distance = FVector::Distance(AtLocation + OtherSocket->GetRelativeLocation(), Socket->GetComponentLocation());
			Distance < BestDistance && Function(OtherSocket,Socket))
		{ 
			BestDistance = Distance;
			OutSocket = Socket;
			bHasFound = true;
		}		
	}

	return bHasFound;
}


 

bool ABuildable_Base::TrySnapToClosestBuildableSocket(ABuildable_Base* OtherBuilding,const FVector& AtLocation, UBuildableSocket*& OutSocket,
	UBuildableSocket*& OutOtherSocket, const TFunction<bool(UBuildableSocket*, UBuildableSocket*)>& Predicate)
{
	if (OtherBuilding == this)
		return false;
	
	bool bHasFound = false;
	float BestSnapDistance = TNumericLimits<int>::Max();

	for (UBuildableSocket* Socket : Execute_IF_GetAvailableSockets(this))
	{
		UBuildableSocket* CompatibleSocket = nullptr;
		
		if (OtherBuilding->GetClosestSocket(Socket, BestSnapDistance,AtLocation,CompatibleSocket, Predicate))
		{
			OutSocket = Socket;
			OutOtherSocket = CompatibleSocket;

			BestSnapDistance = FVector::Distance(AtLocation + OutSocket->GetRelativeLocation(), CompatibleSocket->GetComponentLocation());
			bHasFound = true;
		}
	}
	
	return bHasFound;
}

FVector ABuildable_Base::GetSnapPosition(UBuildableSocket* Socket, UBuildableSocket* OtherSocket)
{
	if (!Socket || !OtherSocket)
	{
		return FVector::ZeroVector;
	}
 
	FVector SocketWorldLocation = Socket->GetComponentLocation();
	FVector OtherSocketWorldLocation = OtherSocket->GetComponentLocation();
     
	FVector ActorLocation = GetActorLocation();
     
	FVector OffsetToSocket = SocketWorldLocation - ActorLocation;
     
	FVector NewActorLocation = OtherSocketWorldLocation - OffsetToSocket;
    
	return NewActorLocation;
}


FTransform ABuildable_Base::GetSnapTransform(UBuildableSocket* Socket, UBuildableSocket* OtherSocket)
{
	if (!Socket || !OtherSocket)
	{
		return FTransform::Identity;
	}
	
	FTransform OtherSocketWorld = OtherSocket->GetComponentTransform();
    
	// Flip 180° for face-to-face
	FQuat Flip = FQuat(FVector::UpVector, PI);
	FTransform FlipTransform(Flip);
	FTransform OtherSocketFlipped = FlipTransform * OtherSocketWorld;
    
	// Get socket transform RELATIVE TO ACTOR ROOT (not mesh!)
	FTransform ActorWorld = GetActorTransform();
	FTransform SocketWorld = Socket->GetComponentTransform();
	FTransform SocketRelativeToActor = SocketWorld.GetRelativeTransform(ActorWorld);
    
	return OtherSocketFlipped * SocketRelativeToActor.Inverse();
}

