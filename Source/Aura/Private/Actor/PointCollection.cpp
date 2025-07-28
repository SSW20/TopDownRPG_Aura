// Copyright Druid Mechanics


#include "Actor/PointCollection.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
APointCollection::APointCollection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Pt_0 = CreateDefaultSubobject<USceneComponent>("Pt_0");
	ImmutablePoints.Add(Pt_0);
	SetRootComponent(Pt_0);

	Pt_1 = CreateDefaultSubobject<USceneComponent>("Pt_1");
	ImmutablePoints.Add(Pt_1);
	Pt_1->SetupAttachment(GetRootComponent());

	Pt_2 = CreateDefaultSubobject<USceneComponent>("Pt_2");
	ImmutablePoints.Add(Pt_2);
	Pt_2->SetupAttachment(GetRootComponent());

	Pt_3 = CreateDefaultSubobject<USceneComponent>("Pt_3");
	ImmutablePoints.Add(Pt_3);
	Pt_3->SetupAttachment(GetRootComponent());
	
	Pt_4 = CreateDefaultSubobject<USceneComponent>("Pt_4");
	ImmutablePoints.Add(Pt_4);
	Pt_4->SetupAttachment(GetRootComponent());

	Pt_5 = CreateDefaultSubobject<USceneComponent>("Pt_5");
	ImmutablePoints.Add(Pt_5);
	Pt_5->SetupAttachment(GetRootComponent());
	
	Pt_6 = CreateDefaultSubobject<USceneComponent>("Pt_6");
	ImmutablePoints.Add(Pt_6);
	Pt_6->SetupAttachment(GetRootComponent());

	Pt_7 = CreateDefaultSubobject<USceneComponent>("Pt_7");
	ImmutablePoints.Add(Pt_7);
	Pt_7->SetupAttachment(GetRootComponent());

	Pt_8 = CreateDefaultSubobject<USceneComponent>("Pt_8");
	ImmutablePoints.Add(Pt_8);
	Pt_8->SetupAttachment(GetRootComponent());

	Pt_9 = CreateDefaultSubobject<USceneComponent>("Pt_9");
	ImmutablePoints.Add(Pt_9);
	Pt_9->SetupAttachment(GetRootComponent());

	Pt_10 = CreateDefaultSubobject<USceneComponent>("Pt_10");
	ImmutablePoints.Add(Pt_10);
	Pt_10->SetupAttachment(GetRootComponent());
}

TArray<USceneComponent*> APointCollection::GetGroundPoints(const FVector& GroundLocation, int32 NumPoints,
	float YawOverride)
{
	checkf(ImmutablePoints.Num() >= NumPoints, TEXT("Attempted to access ImmutablePts out of bounds."));

	TArray<USceneComponent*> GroundPoints;
	for (USceneComponent* Pt : ImmutablePoints)
	{
		if (GroundPoints.Num() >= NumPoints) return GroundPoints;
		if (Pt != Pt_0)
		{
			FVector ToPt_0 = Pt->GetComponentLocation() - Pt_0->GetComponentLocation();
			ToPt_0 = ToPt_0.RotateAngleAxis(YawOverride, FVector::UpVector);
			Pt->SetWorldLocation(ToPt_0 + Pt_0->GetComponentLocation());
		}
		const FVector UpperLocation = FVector(Pt->GetComponentLocation()) + FVector(0,0,500);
		const FVector LowerLocation = FVector(Pt->GetComponentLocation()) + FVector(0,0,-500);
		
		TArray<AActor*> ActorsToIgnore;
		UAuraAbilitySystemLibrary::GetLivePlayersWithInRadius(
			this,
			ActorsToIgnore,
			TArray<AActor*>(),
			500.f,
			GetActorLocation());

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActors(ActorsToIgnore);
		GetWorld()->LineTraceSingleByProfile(HitResult, UpperLocation, LowerLocation, FName("BlockAll"), Params);

		const FVector FinalLocation = FVector(Pt->GetComponentLocation().X,Pt->GetComponentLocation().Y, HitResult.ImpactPoint.Z);
		Pt->SetWorldLocation(FinalLocation);
		Pt->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));

		GroundPoints.Add(Pt);
	}
	return GroundPoints;
}

// Called when the game starts or when spawned
void APointCollection::BeginPlay()
{
	Super::BeginPlay();
	
}


