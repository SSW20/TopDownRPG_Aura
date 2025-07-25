// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"
class UNiagaraComponent;
class UCameraComponent;
class USpringArmComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()
public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/*	Combat Interface */
	virtual int32 GetPlayerLevel_Implementation() const override;
	/*	End Combat Interface */
	
	/*	Player Interface */
	virtual void AddExp_Implementation(int32 Exp) override;
	virtual void LevelUp_Implementation() override;
	virtual void AddLevel_Implementation(int32 Level) override;
	virtual int32 GetExp_Implementation() const override;
	virtual int32 GetLevelByExp_Implementation(int32 Exp) override;
	virtual int32 GetAttributePointReward_Implementation(int32 Level) const override;
	virtual void AddAttributePoint_Implementation(int32 Point) override;
	virtual int32 GetSkillPointReward_Implementation(int32 Level) const override;
	virtual void AddSkillPoint_Implementation(int32 Point) override;
	virtual int32 GetSkillPoint_Implementation() const override;
	virtual int32 GetAttributePoint_Implementation() const override;
	/*	End Player Interface */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

	virtual void OnRep_Stunned() override;
protected:
	virtual void BeginPlay() override;

private:
	virtual void InitAbilityActorInfo() override;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraSpringArm;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const;
};
