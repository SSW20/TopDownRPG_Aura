// Copyright Druid Mechanics


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	// 액터의 Vector 가져오기
	const FVector FowardVector = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FRotator Rotation = GetAvatarActorFromActorInfo()->GetActorRotation();

	const float DeltaSpread = SummonSpread / SummonCount;
	FVector LeftSpread = FowardVector.RotateAngleAxis(SummonSpread / 2.f, FVector::UpVector);
	TArray<FVector> SummonLocations;

	for (int32 index = 1; index <= SummonCount; ++index)
	{
		FVector SummonDirection = LeftSpread.RotateAngleAxis(-DeltaSpread*index, FVector::UpVector);
		float RandDistance = FMath::RandRange(MinSummonDistance, MaxSummonDistance);
		FVector SummonLocation = Location + SummonDirection * RandDistance;

		// 경사진 곳에서도 무조건 바닥에서 나올 수 있게 설정
		FHitResult SummonHit;
		bool bIsHit = GetWorld()->LineTraceSingleByChannel(SummonHit, SummonLocation+FVector(0,0,400.f),
			SummonLocation - FVector(0,0,400.f), ECollisionChannel::ECC_Visibility);
		if (bIsHit)
		{
			SummonLocation = SummonHit.ImpactPoint;
		}
		FVector DeltaSummonLocation = FVector(0,0,50.f);
		
		SummonLocations.Add(SummonLocation + DeltaSummonLocation);
	}
	return SummonLocations; 
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomSummonPawn() const
{
	int32 Index = FMath::RandRange(0, SummonPawns.Num()-1);
	return SummonPawns[Index];
}
