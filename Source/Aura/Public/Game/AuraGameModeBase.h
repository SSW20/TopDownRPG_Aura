// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LootTiers.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "AuraGameModeBase.generated.h"
class USaveGame;
/**
 * 
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "Character Default Attributes")
	TObjectPtr<UCharacterClassInfo> CharacterInfoClass;

	UPROPERTY(EditDefaultsOnly, Category = "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	void SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex);
	ULoadScreenSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;
	void DeleteSlot(const FString& SlotName, int32 SlotIndex);
	void TravelMap(UMVVM_LoadSlot* LoadSlot);
	
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UPROPERTY(EditDefaultsOnly, Category = "Save Data")
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;
	
	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName = "Dungeon 1";

	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapAssetName = "StartMap";

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSoftObjectPtr<UWorld>> Maps;

	UPROPERTY(EditDefaultsOnly)
	FName DefaultPlayerStart;

	UPROPERTY(EditDefaultsOnly, Category = "Loot Tiers")
	TObjectPtr<ULootTiers> LootTiers;

	ULoadScreenSaveGame* RetrieveInGameSaveData();
	void SaveInGameProgressData(ULoadScreenSaveGame* SaveObject);
	FString GetMapNameFromMapAssetName(const FString& MapAssetName) const;
	void SaveWorldState(UWorld* World, const FString& DestinationMapAssetName = FString("")) const;
	void LoadWorldState(UWorld* World) const;
	void PlayerDied(ACharacter* Player);
protected:
	virtual void BeginPlay() override;

};
