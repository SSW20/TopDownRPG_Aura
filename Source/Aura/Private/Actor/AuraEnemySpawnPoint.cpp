// Copyright Druid Mechanics


#include "Actor/AuraEnemySpawnPoint.h"


#include "Character/AuraEnemy.h"

void AAuraEnemySpawnPoint::SpawnEnemy()
{
	FActorSpawnParameters SpawnParams;
	// 조정 후 스폰
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AAuraEnemy* Enemy = GetWorld()->SpawnActorDeferred<AAuraEnemy>(EnemyClass, GetActorTransform());
	Enemy->SetLevel(EnemyLevel);
	Enemy->SetCharacterClass(CharacterClass);
	Enemy->FinishSpawning(GetActorTransform());
	Enemy->SpawnDefaultController();
}



