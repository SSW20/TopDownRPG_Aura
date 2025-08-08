// Copyright Druid Mechanics


#include "Checkpoint/MapEntrance.h"

#include "Components/SphereComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

AMapEntrance::AMapEntrance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Sphere->SetupAttachment(MoveToComponent);
}

void AMapEntrance::HighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);
}

void AMapEntrance::LoadActor_Implementation()
{
	// Do nothing when loading a Map Entrance
}

// 똑같은 행동이지만 목적지 맵의 이름과 PlayerStartTag를 추적하고 직접 맵 이동
void AMapEntrance::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;

		if (AAuraGameModeBase* AuraGM = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			AuraGM->SaveWorldState(GetWorld(), DestinationMap.ToSoftObjectPath().GetAssetName());
		}
		
		IPlayerInterface::Execute_SaveProgress(OtherActor, DestinationPlayerStartTag);

		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap);
	}
}