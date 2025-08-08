// Copyright Druid Mechanics


#include "Checkpoint/Checkpoint.h"

// Copyright Druid Mechanics


#include "Checkpoint/Checkpoint.h"

#include "Components/SphereComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

ACheckpoint::ACheckpoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	
	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>("CheckpointMesh");
	CheckpointMesh->SetupAttachment(GetRootComponent());
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CheckpointMesh->SetCollisionResponseToAllChannels(ECR_Block);

	// Pawn이랑만 Overlap
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(CheckpointMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Highlight에 필요한 것들 설정
	CheckpointMesh->SetCustomDepthStencilValue(CustomDepthStencilOverride);
	CheckpointMesh->MarkRenderStateDirty();

	MoveToComponent = CreateDefaultSubobject<USceneComponent>("MoveToComponent");
	MoveToComponent->SetupAttachment(GetRootComponent());
}

void ACheckpoint::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = MoveToComponent->GetComponentLocation();
}

void ACheckpoint::HighlightActor_Implementation()
{
	if (!bReached)
	{
		CheckpointMesh->SetRenderCustomDepth(true);
	}
}

void ACheckpoint::UnHighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(false);
}

void ACheckpoint::LoadActor_Implementation()
{
	if (bReached)
	{
		ActivateGlowEffect();
	}
}

void ACheckpoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 겹친 액터가 Player 태그를 가질 시 Glow
	if (OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;

		if (AAuraGameModeBase* AuraGM = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			UWorld* World = GetWorld();
			FString WorldName = World->GetMapName();
			WorldName.RemoveFromStart(World->StreamingLevelsPrefix);
			
			AuraGM->SaveWorldState(GetWorld(), WorldName);
		}
		IPlayerInterface::Execute_SaveProgress(OtherActor,PlayerStartTag);
		ActivateGlowEffect();
	}
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	// Overlap Call Back 바인드
	if (bBindOverlapCallback)
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnSphereOverlap);
	}
}

void ACheckpoint::ActivateGlowEffect()
{
	// 나중에 Material Instance 를 Timeline을 통해 반짝이게 만듦
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UMaterialInstanceDynamic* DynamicMaterialInstace = UMaterialInstanceDynamic::Create(CheckpointMesh->GetMaterial(0), this);
	CheckpointMesh->SetMaterial(0, DynamicMaterialInstace);
	ReachedCheckpoint(DynamicMaterialInstace);
}