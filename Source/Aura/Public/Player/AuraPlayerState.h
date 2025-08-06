// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "AuraPlayerState.generated.h"


class UAbilitySystemComponent;
class UAttributeSet;
/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerStatChange, int32);
DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerLevelChange, int32, bool);

UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AAuraPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/*		Level		*/
	FORCEINLINE int32 GetPlayerLevel() const { return Level; };
	void SetLevel(int32 NewLevel);
	void AddToLevel(int32 NewLevel);
	void SetLevelWithLoad(int32 NewLevel);
	FPlayerLevelChange LevelChangeDelegate;

	/*		Exp		*/
	void SetExp(int32 NewExp);
	void AddToExp(int32 NewExp);
	FORCEINLINE int32 GetExp() const { return Exp; };
	FPlayerStatChange ExpChangeDelegate;

	/*		AttributePoint		*/
	FPlayerStatChange AttributePointChangeDelegate;
	FORCEINLINE int32 GetAttributePoint() const { return AttributePoint; };
	void AddAttributePoint(int32 NewAttributePoint);
	void SetAttributePoint(int32 NewAttributePoint);

	/*		SkillPoint			*/
	FPlayerStatChange SkillPointChangeDelegate;
	FORCEINLINE int32 GetSkillPoint() const { return SkillPoint; };
	void AddSkillPoint(int32 NewSkillPoint);
	void SetSkillPoint(int32 NewSkillPoint);
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;

	
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Exp)
	int32 Exp = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_AttributePoint)
	int32 AttributePoint = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SkillPoint)
	int32 SkillPoint = 1;
	
	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UFUNCTION()
	void OnRep_Exp(int32 OldExp);

	UFUNCTION()
	void OnRep_AttributePoint(int32 OldAttributePoint);

	UFUNCTION()
	void OnRep_SkillPoint(int32 OldSkillPoint);
};
